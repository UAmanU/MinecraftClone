#include "Game/world.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include "Config/world_config.h"

std::size_t ChunkCoordHash::operator()(const ChunkCoord& coord) const noexcept {
    const auto x = static_cast<std::uint32_t>(coord.x);
    const auto z = static_cast<std::uint32_t>(coord.z);
    return (std::hash<std::uint32_t>{}(x) << 1U) ^ std::hash<std::uint32_t>{}(z);
}

std::size_t World::BlockCoordHash::operator()(const BlockCoord& pos) const noexcept {
    std::size_t h = std::hash<int>{}(pos.x);
    h ^= std::hash<int>{}(pos.y) + 0x9e3779b9U + (h << 6U) + (h >> 2U);
    h ^= std::hash<int>{}(pos.z) + 0x9e3779b9U + (h << 6U) + (h >> 2U);
    return h;
}

int World::FloorDiv(int value, int divisor) {
    int quotient = value / divisor;
    if (value % divisor < 0) --quotient;
    return quotient;
}

int World::Local(int value, int divisor) {
    const int remainder = value % divisor;
    return remainder < 0 ? remainder + divisor : remainder;
}

std::size_t World::Index(int x, int y, int z) {
    return static_cast<std::size_t>((y * WorldConfig::CHUNK_SIZE + x) * WorldConfig::CHUNK_SIZE + z);
}

unsigned World::Noise(int x, int z) {
    std::uint32_t value = static_cast<std::uint32_t>(x) * 0x8da6b343U
        ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ 0xcb1ab31fU;
    value ^= value >> 13U;
    value *= 0x85ebca6bU;
    value ^= value >> 16U;
    return value;
}

int World::TerrainHeight(int x, int z) {
    return WorldConfig::BASE_TERRAIN_HEIGHT + static_cast<int>(std::round(
        std::sin(x * 0.035F) * 1.0F + std::cos(z * 0.041F) * 1.0F
        + std::sin((x + z) * 0.019F) * 0.7F));
}

Chunk& World::EnsureChunk(ChunkCoord coordinate) {
    auto [it, inserted] = chunks_.try_emplace(coordinate);
    if (inserted) {
        it->second.coordinate = coordinate;
        it->second.blocks.assign(static_cast<std::size_t>(WorldConfig::CHUNK_SIZE * WorldConfig::CHUNK_SIZE * WorldConfig::WORLD_HEIGHT), BlockType::Air);
        GenerateChunk(it->second);
    }
    return it->second;
}

void World::GenerateChunk(Chunk& chunk) {
    const int origin_x = chunk.coordinate.x * WorldConfig::CHUNK_SIZE;
    const int origin_z = chunk.coordinate.z * WorldConfig::CHUNK_SIZE;
    for (int lx=0; lx<WorldConfig::CHUNK_SIZE; ++lx) for (int lz=0; lz<WorldConfig::CHUNK_SIZE; ++lz) {
        const int x=origin_x+lx, z=origin_z+lz;
        const int height=TerrainHeight(x,z);
        for (int y=0; y<=height && y<WorldConfig::WORLD_HEIGHT; ++y) {
            chunk.blocks[Index(lx,y,lz)] = y==height ? BlockType::Grass
                : y>=height-WorldConfig::DIRT_LAYER_DEPTH ? BlockType::Dirt : BlockType::Stone;
        }
        const int base_x=FloorDiv(x,6)*6;
        const int base_z=FloorDiv(z,7)*7;
        for (int ax=base_x-6; ax<=base_x+6; ax+=6) for (int az=base_z-7; az<=base_z+7; az+=7) {
            if (Noise(ax,az)%5U==0U) continue;
            const int ground=TerrainHeight(ax,az);
            for (int dy=1; dy<=4; ++dy) {
                if (x==ax && z==az && ground+dy<WorldConfig::WORLD_HEIGHT)
                    chunk.blocks[Index(lx,ground+dy,lz)] = BlockType::OakLog;
            }
            for (int dx=-2; dx<=2; ++dx) for (int dz=-2; dz<=2; ++dz) for (int dy=2; dy<=5; ++dy) {
                const int tx=ax+dx, tz=az+dz, ty=ground+dy;
                if (tx!=x || tz!=z || ty>=WorldConfig::WORLD_HEIGHT || std::abs(dx)+std::abs(dz)+std::max(0,dy-4)>4) continue;
                if (chunk.blocks[Index(lx,ty,lz)]==BlockType::Air) chunk.blocks[Index(lx,ty,lz)] = BlockType::Leaves;
            }
        }
    }
    for (const auto& [pos,type] : edits_) {
        if (FloorDiv(pos.x,WorldConfig::CHUNK_SIZE)==chunk.coordinate.x && FloorDiv(pos.z,WorldConfig::CHUNK_SIZE)==chunk.coordinate.z)
            chunk.blocks[Index(Local(pos.x,WorldConfig::CHUNK_SIZE),pos.y,Local(pos.z,WorldConfig::CHUNK_SIZE))]=type;
    }
}

void World::UpdateAround(float world_x, float world_z) {
    const ChunkCoord center{FloorDiv(static_cast<int>(std::floor(world_x+0.5F)),WorldConfig::CHUNK_SIZE),
                            FloorDiv(static_cast<int>(std::floor(world_z+0.5F)),WorldConfig::CHUNK_SIZE)};
    if (has_center_ && center.x==center_.x && center.z==center_.z) return;
    center_=center;
    has_center_=true;
    const int radius=WorldConfig::LOAD_RADIUS_CHUNKS;
    for(int x=center.x-radius;x<=center.x+radius;++x) for(int z=center.z-radius;z<=center.z+radius;++z) EnsureChunk({x,z});
    for(auto it=chunks_.begin();it!=chunks_.end();) {
        if(std::abs(it->first.x-center.x)>radius || std::abs(it->first.z-center.z)>radius) it=chunks_.erase(it);
        else ++it;
    }
}

BlockType World::GetBlock(int x,int y,int z) const {
    if(y<0) return BlockType::Stone;
    if(y>=WorldConfig::WORLD_HEIGHT) return BlockType::Air;
    const auto edit=edits_.find({x,y,z});
    if(edit!=edits_.end()) return edit->second;
    const auto chunk=chunks_.find({FloorDiv(x,WorldConfig::CHUNK_SIZE),FloorDiv(z,WorldConfig::CHUNK_SIZE)});
    if(chunk==chunks_.end()) return BlockType::Air;
    return chunk->second.blocks[Index(Local(x,WorldConfig::CHUNK_SIZE),y,Local(z,WorldConfig::CHUNK_SIZE))];
}

void World::SetBlock(int x,int y,int z,BlockType type) {
    if(y<0||y>=WorldConfig::WORLD_HEIGHT) return;
    edits_[{x,y,z}]=type;
    const auto chunk=chunks_.find({FloorDiv(x,WorldConfig::CHUNK_SIZE),FloorDiv(z,WorldConfig::CHUNK_SIZE)});
    if(chunk!=chunks_.end()) chunk->second.blocks[Index(Local(x,WorldConfig::CHUNK_SIZE),y,Local(z,WorldConfig::CHUNK_SIZE))]=type;
}

float World::SurfaceTop(int x,int z) const {
    for(int y=WorldConfig::WORLD_HEIGHT-1;y>=0;--y) if(GetBlock(x,y,z)!=BlockType::Air) return static_cast<float>(y)+0.5F;
    return 0.5F;
}
