#include "Game/game_renderer.h"

#include <cmath>

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
                DrawCube(position, 1.0F, 1.0F, 1.0F, BlockColor(block));
                DrawCubeWires(position, 1.0F, 1.0F, 1.0F, Color{30, 40, 30, 80});
            }
        }
    }
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