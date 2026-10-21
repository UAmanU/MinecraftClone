#include "Game/game_renderer.h"

#include <cmath>
#include <algorithm>

#include "Config/mining_config.h"
#include "Config/world_config.h"
#include "Entity/Object/Player/player.h"
#include "Config/math_utils.h"

void GameRenderer::DrawWorld(const std::vector<BlockType>& world, const Player& player,
                             const std::vector<ItemDrop>& drops, const std::vector<Pig>& pigs) const {
    BeginMode3D(player.Camera());
    for (int x = 0; x < WorldConfig::WORLD_SIZE; ++x) {
        for (int y = 0; y < WorldConfig::WORLD_HEIGHT; ++y) {
            for (int z = 0; z < WorldConfig::WORLD_SIZE; ++z) {
                const std::size_t index = static_cast<std::size_t>(
                    x * WorldConfig::WORLD_SIZE * WorldConfig::WORLD_HEIGHT
                    + y * WorldConfig::WORLD_SIZE + z);
                const BlockType block = world[index];
                if (block == BlockType::Air) continue;
                const Vector3 position{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
                bool exposed=false;
                constexpr int offsets[6][3]{{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
                for(const auto& offset:offsets){const int nx=x+offset[0],ny=y+offset[1],nz=z+offset[2];if(nx<0||nx>=WorldConfig::WORLD_SIZE||ny<0||ny>=WorldConfig::WORLD_HEIGHT||nz<0||nz>=WorldConfig::WORLD_SIZE||world[static_cast<std::size_t>(nx*WorldConfig::WORLD_SIZE*WorldConfig::WORLD_HEIGHT+ny*WorldConfig::WORLD_SIZE+nz)]==BlockType::Air){exposed=true;break;}}
                if(!exposed)continue;
                Color tint=BlockColor(block);const unsigned noise=static_cast<unsigned>(x*73856093u)^static_cast<unsigned>(y*19349663u)^static_cast<unsigned>(z*83492791u);const int shade=static_cast<int>(noise%19u)-9;
                tint.r=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.r)+shade,0,255));tint.g=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.g)+shade,0,255));tint.b=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.b)+shade,0,255));
                DrawCube(position,1.0F,1.0F,1.0F,tint);DrawCubeWires(position,1.0F,1.0F,1.0F,Color{30,40,30,55});
            }
        }
    }
    for(int cloud=0;cloud<9;++cloud){const float cx=static_cast<float>((cloud*13+5)%WorldConfig::WORLD_SIZE),cz=static_cast<float>((cloud*19+8)%WorldConfig::WORLD_SIZE),cy=static_cast<float>(WorldConfig::WORLD_HEIGHT+3+(cloud%2));DrawCube({cx,cy,cz},5.0F,0.7F,2.0F,Color{248,251,255,255});DrawCube({cx+2.0F,cy+0.3F,cz},2.5F,0.8F,2.0F,Color{248,251,255,255});}
    for (const Pig& pig : pigs) pig.Draw();
    for (const ItemDrop& drop : drops) {
        DrawCube(drop.position, MiningConfig::DROP_SIZE, MiningConfig::DROP_SIZE,
                 MiningConfig::DROP_SIZE, BlockColor(drop.type));
    }
    EndMode3D();
}

bool GameRenderer::Raycast(const std::vector<BlockType>& world, const Camera3D& camera,
                           Vector3& hit, Vector3& adjacent) const {
    const Vector3 direction = Math3D::Normalize(Math3D::Subtract(camera.target, camera.position));
    for (float distance = 0.0F; distance <= WorldConfig::REACH; distance += 0.08F) {
        const Vector3 point = Math3D::Add(camera.position, Math3D::Scale(direction, distance));
        const int x = static_cast<int>(std::floor(point.x + 0.5F));
        const int y = static_cast<int>(std::floor(point.y + 0.5F));
        const int z = static_cast<int>(std::floor(point.z + 0.5F));
        if (x < 0 || x >= WorldConfig::WORLD_SIZE || y < 0 || y >= WorldConfig::WORLD_HEIGHT
            || z < 0 || z >= WorldConfig::WORLD_SIZE) continue;
        const std::size_t index = static_cast<std::size_t>(
            x * WorldConfig::WORLD_SIZE * WorldConfig::WORLD_HEIGHT + y * WorldConfig::WORLD_SIZE + z);
        if (world[index] == BlockType::Air) continue;

        hit = Vector3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
        const Vector3 previous = Math3D::Add(camera.position, Math3D::Scale(direction, distance - 0.08F));
        adjacent = Vector3{
            std::floor(previous.x + 0.5F),
            std::floor(previous.y + 0.5F),
            std::floor(previous.z + 0.5F)};
        return true;
    }
    return false;
}