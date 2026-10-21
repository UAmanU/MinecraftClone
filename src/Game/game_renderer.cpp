#include "Game/game_renderer.h"

#include <algorithm>
#include <cmath>
#include "Config/game_config.h"
#include "Config/mining_config.h"
#include "Config/world_config.h"
#include "Config/math_utils.h"
#include "Entity/Object/Player/player.h"
#include "GUI/gui_renderer.h"
#include "raylib.h"

void GameRenderer::DrawFrame(const World& world, const Player& player,
                             const std::vector<ItemDrop>& drops, const std::vector<Pig>& pigs,
                             bool inventory_open, bool crafting_open, float mining_progress) const {
    ClearBackground(SKYBLUE);
    DrawWorld(world, player, drops, pigs);
    GuiRenderer().Draw(player, inventory_open);
    if (crafting_open) DrawCraftingMenu();
    if (mining_progress >= 0.0F) DrawMiningProgress(mining_progress);
}

void GameRenderer::DrawWorld(const World& world, const Player& player,
                             const std::vector<ItemDrop>& drops, const std::vector<Pig>& pigs) const {
    const Camera3D& camera=player.Camera();
    BeginMode3D(camera);
    constexpr float render_distance=43.0F;
    const float render_distance_squared=render_distance*render_distance;
    constexpr int offsets[6][3]{{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    for(const auto& [chunk_coord,chunk]:world.LoadedChunks()) {
        const float chunk_x=static_cast<float>(chunk_coord.x*WorldConfig::CHUNK_SIZE);
        const float chunk_z=static_cast<float>(chunk_coord.z*WorldConfig::CHUNK_SIZE);
        const float closest_x=std::clamp(camera.position.x,chunk_x,chunk_x+WorldConfig::CHUNK_SIZE);
        const float closest_z=std::clamp(camera.position.z,chunk_z,chunk_z+WorldConfig::CHUNK_SIZE);
        const float dx=closest_x-camera.position.x,dz=closest_z-camera.position.z;
        if(dx*dx+dz*dz>render_distance_squared) continue;
        for(int lx=0;lx<WorldConfig::CHUNK_SIZE;++lx) for(int y=0;y<WorldConfig::WORLD_HEIGHT;++y) for(int lz=0;lz<WorldConfig::CHUNK_SIZE;++lz) {
            const BlockType block=chunk.blocks[static_cast<std::size_t>((y*WorldConfig::CHUNK_SIZE+lx)*WorldConfig::CHUNK_SIZE+lz)];
            if(block==BlockType::Air) continue;
            const int x=chunk_coord.x*WorldConfig::CHUNK_SIZE+lx,z=chunk_coord.z*WorldConfig::CHUNK_SIZE+lz;
            const float vx=static_cast<float>(x)-camera.position.x,vz=static_cast<float>(z)-camera.position.z;
            if(vx*vx+vz*vz>render_distance_squared) continue;
            bool exposed=false;
            for(const auto& offset:offsets) if(world.GetBlock(x+offset[0],y+offset[1],z+offset[2])==BlockType::Air){exposed=true;break;}
            if(!exposed) continue;
            Color tint=BlockColor(block);
            const unsigned noise=static_cast<unsigned>(x*73856093u)^static_cast<unsigned>(y*19349663u)^static_cast<unsigned>(z*83492791u);
            const int shade=static_cast<int>(noise%19U)-9;
            tint.r=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.r)+shade,0,255));
            tint.g=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.g)+shade,0,255));
            tint.b=static_cast<unsigned char>(std::clamp(static_cast<int>(tint.b)+shade,0,255));
            DrawCube({static_cast<float>(x),static_cast<float>(y),static_cast<float>(z)},1,1,1,tint);
        }
    }
    const int cloud_x=static_cast<int>(std::floor(camera.position.x/32.0F))*32;
    const int cloud_z=static_cast<int>(std::floor(camera.position.z/32.0F))*32;
    for(int cx=-2;cx<=2;++cx) for(int cz=-2;cz<=2;++cz) {
        const float x=static_cast<float>(cloud_x+cx*32),z=static_cast<float>(cloud_z+cz*32),y=static_cast<float>(WorldConfig::WORLD_HEIGHT+4);
        DrawCube({x,y,z},6.0F,0.8F,2.4F,Color{248,251,255,255});
        DrawCube({x+2.3F,y+0.3F,z},3.0F,0.7F,2.2F,Color{248,251,255,255});
    }
    for(const Pig& pig:pigs) pig.Draw();
    for(const ItemDrop& drop:drops) DrawCube(drop.position,MiningConfig::DROP_SIZE,MiningConfig::DROP_SIZE,MiningConfig::DROP_SIZE,BlockColor(drop.type));
    EndMode3D();
}

bool GameRenderer::Raycast(const World& world,const Camera3D& camera,Vector3& hit,Vector3& adjacent) const {
    const Vector3 direction=Math3D::Normalize(Math3D::Subtract(camera.target,camera.position));
    for(float distance=0;distance<=WorldConfig::REACH;distance+=0.06F) {
        const Vector3 point=Math3D::Add(camera.position,Math3D::Scale(direction,distance));
        const int x=static_cast<int>(std::floor(point.x+0.5F)),y=static_cast<int>(std::floor(point.y+0.5F)),z=static_cast<int>(std::floor(point.z+0.5F));
        if(y<0||y>=WorldConfig::WORLD_HEIGHT||world.GetBlock(x,y,z)==BlockType::Air) continue;
        hit={static_cast<float>(x),static_cast<float>(y),static_cast<float>(z)};
        const Vector3 previous=Math3D::Add(camera.position,Math3D::Scale(direction,distance-0.06F));
        adjacent={std::floor(previous.x+0.5F),std::floor(previous.y+0.5F),std::floor(previous.z+0.5F)};
        return true;
    }
    return false;
}

void GameRenderer::DrawCraftingMenu() const {
    DrawRectangle(GameConfig::SCREEN_WIDTH/2-290,190,580,360,Color{66,53,43,245});
    DrawRectangleLines(GameConfig::SCREEN_WIDTH/2-290,190,580,360,RAYWHITE);
    DrawText("CRAFTING TABLE",GameConfig::SCREEN_WIDTH/2-135,210,32,WHITE);
    DrawText("K  Sticks: 2 Planks",GameConfig::SCREEN_WIDTH/2-230,270,22,RAYWHITE);
    DrawText("P  Pickaxe: 3 Planks + 2 Sticks",GameConfig::SCREEN_WIDTH/2-230,310,22,RAYWHITE);
    DrawText("A  Axe: 3 Planks + 2 Sticks",GameConfig::SCREEN_WIDTH/2-230,350,22,RAYWHITE);
    DrawText("H  Shovel: 1 Plank + 2 Sticks",GameConfig::SCREEN_WIDTH/2-230,390,22,RAYWHITE);
    DrawText("S  Sword: 2 Planks + 1 Stick",GameConfig::SCREEN_WIDTH/2-230,430,22,RAYWHITE);
    DrawText("Escape to close",GameConfig::SCREEN_WIDTH/2-75,500,18,LIGHTGRAY);
}

void GameRenderer::DrawMiningProgress(float progress) const {
    const int x=GameConfig::SCREEN_WIDTH/2-90,y=GameConfig::SCREEN_HEIGHT/2+24;
    DrawRectangle(x,y,180,12,Color{0,0,0,180});
    DrawRectangle(x+2,y+2,static_cast<int>(176.0F*std::clamp(progress,0.0F,1.0F)),8,GOLD);
}
