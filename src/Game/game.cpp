#include "Game/game.h"

#include <algorithm>
#include <cmath>

#include "Config/game_config.h"
#include "Config/math_utils.h"
#include "Config/mining_config.h"
#include "Config/player_config.h"
#include "Config/world_config.h"
#include "GUI/gui_renderer.h"
#include "raylib.h"

Game::Game()
    : renderer_(std::make_unique<GameRenderer>()),
      player_(std::make_unique<Player>(Vector3{
          static_cast<float>(WorldConfig::WORLD_SIZE / 2),
          static_cast<float>(WorldConfig::BASE_TERRAIN_HEIGHT + 2),
          static_cast<float>(WorldConfig::WORLD_SIZE / 2)})),
      world_(static_cast<std::size_t>(WorldConfig::WORLD_SIZE * WorldConfig::WORLD_HEIGHT * WorldConfig::WORLD_SIZE),
             BlockType::Air) {
    Generate();
    pigs_.emplace_back(Vector3{20.0F, 2.0F, 23.0F});
}

int Game::Index(int x, int y, int z) const {
    return x * WorldConfig::WORLD_SIZE * WorldConfig::WORLD_HEIGHT
        + y * WorldConfig::WORLD_SIZE + z;
}

void Game::Generate() {
    for (int x = 0; x < WorldConfig::WORLD_SIZE; ++x) {
        for (int z = 0; z < WorldConfig::WORLD_SIZE; ++z) {
            const int height = WorldConfig::BASE_TERRAIN_HEIGHT + static_cast<int>(
                std::round(std::sin(x * 0.22F) * 0.7F + std::cos(z * 0.19F) * 0.7F));
            for (int y = 0; y <= height; ++y) {
                world_[static_cast<std::size_t>(Index(x, y, z))] =
                    y == height ? BlockType::Grass
                    : y >= height - WorldConfig::DIRT_LAYER_DEPTH ? BlockType::Dirt
                    : BlockType::Stone;
            }
        }
    }

    for (int x = 4; x < WorldConfig::WORLD_SIZE - 4; x += 5) {
        for (int z = 4; z < WorldConfig::WORLD_SIZE - 4; z += 6) {
            if (GetRandomValue(0, 2) == 0) {
                continue;
            }
            int ground = 2;
            while (ground < WorldConfig::WORLD_HEIGHT - 6
                   && world_[static_cast<std::size_t>(Index(x, ground + 1, z))] != BlockType::Air) {
                ++ground;
            }
            for (int y = ground + 1; y <= ground + 4; ++y) {
                world_[static_cast<std::size_t>(Index(x, y, z))] = BlockType::OakLog;
            }
            for (int dx = -2; dx <= 2; ++dx) {
                for (int dz = -2; dz <= 2; ++dz) {
                    for (int dy = 2; dy <= 5; ++dy) {
                        const int leaf_x = x + dx;
                        const int leaf_y = ground + dy;
                        const int leaf_z = z + dz;
                        if (leaf_y < WorldConfig::WORLD_HEIGHT
                            && std::abs(dx) + std::abs(dz) + std::max(0, dy - 4) <= 4
                            && world_[static_cast<std::size_t>(Index(leaf_x, leaf_y, leaf_z))] == BlockType::Air) {
                            world_[static_cast<std::size_t>(Index(leaf_x, leaf_y, leaf_z))] = BlockType::Leaves;
                        }
                    }
                }
            }
        }
    }
}

bool Game::CollidesAt(float x, float camera_y, float z) const {
    const float half = PlayerConfig::PLAYER_WIDTH * 0.5F;
    const float feet = camera_y - PlayerConfig::PLAYER_HEIGHT * 0.88F;
    const float head = feet + PlayerConfig::PLAYER_HEIGHT;
    const int min_x = static_cast<int>(std::floor(x - half + 0.5F));
    const int max_x = static_cast<int>(std::floor(x + half - 0.001F + 0.5F));
    const int min_z = static_cast<int>(std::floor(z - half + 0.5F));
    const int max_z = static_cast<int>(std::floor(z + half - 0.001F + 0.5F));
    const int min_y = static_cast<int>(std::floor(feet + 0.5F));
    const int max_y = static_cast<int>(std::floor(head - 0.001F + 0.5F));
    for (int bx=min_x; bx<=max_x; ++bx) for (int by=min_y; by<=max_y; ++by) for (int bz=min_z; bz<=max_z; ++bz) {
        if (bx<0 || bx>=WorldConfig::WORLD_SIZE || bz<0 || bz>=WorldConfig::WORLD_SIZE || by<0) return true;
        if (by<WorldConfig::WORLD_HEIGHT && world_[static_cast<std::size_t>(Index(bx,by,bz))]!=BlockType::Air) return true;
    }
    return false;
}
bool Game::Craft(BlockType result, std::initializer_list<std::pair<BlockType, int>> recipe) {
    for (const auto& [type, count] : recipe) {
        if (player_->Count(type) < count) {
            return false;
        }
    }
    for (const auto& [type, count] : recipe) {
        player_->Remove(type, count);
    }
    player_->Add(result, result == BlockType::Stick ? MiningConfig::STICKS_PER_RECIPE : 1);
    return true;
}

float Game::MiningDuration(BlockType type) const {
    const auto held = player_->Inventory()[static_cast<std::size_t>(player_->Selected())].type;
    switch (type) {
        case BlockType::OakLog:
            return held == BlockType::Axe
                ? MiningConfig::OAK_LOG_WITH_AXE_SECONDS : MiningConfig::OAK_LOG_SECONDS;
        case BlockType::Grass:
        case BlockType::Dirt:
            return held == BlockType::Shovel
                ? MiningConfig::DIRT_WITH_SHOVEL_SECONDS : MiningConfig::DIRT_SECONDS;
        case BlockType::Leaves:
            return MiningConfig::LEAVES_SECONDS;
        case BlockType::Stone:
            return held == BlockType::Pickaxe
                ? MiningConfig::STONE_WITH_PICKAXE_SECONDS : MiningConfig::STONE_SECONDS;
        default:
            return MiningConfig::DIRT_SECONDS;
    }
}

void Game::Update(float delta_time) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (crafting_open_) {
            crafting_open_ = false;
            return;
        }
        if (inventory_open_) {
            inventory_open_ = false;
            return;
        }
        running_ = false;
        return;
    }

    if (crafting_open_) {
        if (IsKeyPressed(KEY_K)) {
            Craft(BlockType::Stick, {{BlockType::Planks, 2}});
        }
        if (IsKeyPressed(KEY_P)) {
            Craft(BlockType::Pickaxe, {{BlockType::Planks, 3}, {BlockType::Stick, 2}});
        }
        if (IsKeyPressed(KEY_A)) {
            Craft(BlockType::Axe, {{BlockType::Planks, 3}, {BlockType::Stick, 2}});
        }
        if (IsKeyPressed(KEY_H)) {
            Craft(BlockType::Shovel, {{BlockType::Planks, 1}, {BlockType::Stick, 2}});
        }
        if (IsKeyPressed(KEY_S)) {
            Craft(BlockType::Sword, {{BlockType::Planks, 2}, {BlockType::Stick, 1}});
        }
        return;
    }

    if (IsKeyPressed(KEY_E)) {
        inventory_open_ = !inventory_open_;
        return;
    }

    if (inventory_open_) {
        if (IsKeyPressed(KEY_C) && player_->Remove(BlockType::OakLog)) {
            player_->Add(BlockType::Planks, MiningConfig::PLANKS_PER_LOG);
        }
        if (IsKeyPressed(KEY_K)) {
            Craft(BlockType::Stick, {{BlockType::Planks, 2}});
        }
        if (IsKeyPressed(KEY_V)) {
            Craft(BlockType::CraftingTable, {{BlockType::Planks, 4}});
        }
        return;
    }

    auto& camera = player_->Camera();
    const Vector2 mouse_delta = GetMouseDelta();
    yaw_ += mouse_delta.x * PlayerConfig::MOUSE_SENSITIVITY;
    pitch_ = std::clamp(pitch_ - mouse_delta.y * PlayerConfig::MOUSE_SENSITIVITY, -1.52F, 1.52F);
    const Vector3 look_direction{
        std::cos(yaw_) * std::cos(pitch_),
        std::sin(pitch_),
        std::sin(yaw_) * std::cos(pitch_)};
    camera.target = Vector3{
        camera.position.x + look_direction.x,
        camera.position.y + look_direction.y,
        camera.position.z + look_direction.z};

    for (int slot = 0; slot < PlayerConfig::INVENTORY_SLOTS; ++slot) {
        const int key = slot == 9 ? KEY_ZERO : KEY_ONE + slot;
        if (IsKeyPressed(key)) {
            player_->Select(slot);
        }
    }

    const Vector3 forward{std::cos(yaw_), 0.0F, std::sin(yaw_)};
    const Vector3 right = Math3D::Cross(forward, Vector3{0.0F, 1.0F, 0.0F});
    Vector3 movement{};
    if (IsKeyDown(KEY_W)) movement = Math3D::Add(movement, forward);
    if (IsKeyDown(KEY_S)) movement = Math3D::Subtract(movement, forward);
    if (IsKeyDown(KEY_D)) movement = Math3D::Add(movement, right);
    if (IsKeyDown(KEY_A)) movement = Math3D::Subtract(movement, right);
    if (Math3D::Length(movement) > 0.0F) {
        const float speed = IsKeyDown(KEY_LEFT_SHIFT)
            ? PlayerConfig::SPRINT_SPEED : PlayerConfig::WALK_SPEED;
        movement = Math3D::Scale(Math3D::Normalize(movement), speed * delta_time);
        const float next_x = std::clamp(camera.position.x + movement.x, 1.0F, static_cast<float>(WorldConfig::WORLD_SIZE - 2));
        if (!CollidesAt(next_x, camera.position.y, camera.position.z)) camera.position.x = next_x;
        const float next_z = std::clamp(camera.position.z + movement.z, 1.0F, static_cast<float>(WorldConfig::WORLD_SIZE - 2));
        if (!CollidesAt(camera.position.x, camera.position.y, next_z)) camera.position.z = next_z;
    }

    const int block_x = static_cast<int>(std::floor(camera.position.x + 0.5F));
    const int block_z = static_cast<int>(std::floor(camera.position.z + 0.5F));
    float ground_top = 0.5F;
    if (block_x >= 0 && block_x < WorldConfig::WORLD_SIZE
        && block_z >= 0 && block_z < WorldConfig::WORLD_SIZE) {
        for (int y = WorldConfig::WORLD_HEIGHT - 1; y >= 0; --y) {
            const BlockType block = world_[static_cast<std::size_t>(Index(block_x, y, block_z))];
            if (block != BlockType::Air && static_cast<float>(y) + 0.5F <= camera.position.y - PlayerConfig::PLAYER_HEIGHT * 0.88F + 0.08F) {
                ground_top = static_cast<float>(y) + 0.5F;
                break;
            }
        }
    }

    const float previous_camera_y = camera.position.y;
    const float eye_height = PlayerConfig::PLAYER_HEIGHT * 0.88F;
    const float feet_y = camera.position.y - eye_height;
    const bool grounded = vertical_velocity_ <= 0.0F && feet_y <= ground_top + 0.04F;
    if (grounded) {
        if (falling_) {
            const float fall_distance = fall_start_y_ - feet_y;
            if (fall_distance > PlayerConfig::SAFE_FALL_DISTANCE) {
                const int damage = static_cast<int>(std::ceil(fall_distance - PlayerConfig::SAFE_FALL_DISTANCE)) * PlayerConfig::FALL_DAMAGE_PER_BLOCK;
                player_->Damage(damage);
            }
            falling_ = false;
        }
        camera.position.y = ground_top + eye_height;
        vertical_velocity_ = 0.0F;
        if (IsKeyPressed(KEY_SPACE)) vertical_velocity_ = PlayerConfig::JUMP_SPEED;
    } else {
        if (vertical_velocity_ <= 0.0F && !falling_) { falling_ = true; fall_start_y_ = feet_y; }
        vertical_velocity_ -= PlayerConfig::GRAVITY * delta_time;
        const float next_y = camera.position.y + vertical_velocity_ * delta_time;
        if (CollidesAt(camera.position.x, next_y, camera.position.z)) {
            if (vertical_velocity_ < 0.0F) camera.position.y = ground_top + eye_height;
            vertical_velocity_ = 0.0F;
        } else camera.position.y = next_y;
    }
    camera.target.y += camera.position.y - previous_camera_y;
    player_->Update(delta_time);

    Vector3 hit{};
    Vector3 adjacent{};
    const bool has_target = renderer_->Raycast(world_, camera, hit, adjacent);
    if (has_target && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (!mining_active_ || Math3D::Distance(mining_target_, hit) > 0.1F) {
            mining_active_ = true;
            mining_target_ = hit;
            mining_elapsed_ = 0.0F;
        }
        mining_elapsed_ += delta_time;
        const int x = static_cast<int>(hit.x);
        const int y = static_cast<int>(hit.y);
        const int z = static_cast<int>(hit.z);
        const BlockType block = world_[static_cast<std::size_t>(Index(x, y, z))];
        if (mining_elapsed_ >= MiningDuration(block)) {
            world_[static_cast<std::size_t>(Index(x, y, z))] = BlockType::Air;
            drops_.push_back({block, Vector3{hit.x, hit.y + 0.35F, hit.z}, 30.0F});
            mining_active_ = false;
            mining_elapsed_ = 0.0F;
        }
    } else {
        mining_active_ = false;
        mining_elapsed_ = 0.0F;
    }

    if (has_target && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        const int x = static_cast<int>(hit.x);
        const int y = static_cast<int>(hit.y);
        const int z = static_cast<int>(hit.z);
        if (world_[static_cast<std::size_t>(Index(x, y, z))] == BlockType::CraftingTable) {
            crafting_open_ = true;
        } else if (adjacent.x >= 0.0F && adjacent.x < WorldConfig::WORLD_SIZE
                   && adjacent.y >= 0.0F && adjacent.y < WorldConfig::WORLD_HEIGHT
                   && adjacent.z >= 0.0F && adjacent.z < WorldConfig::WORLD_SIZE) {
            const int ax = static_cast<int>(adjacent.x);
            const int ay = static_cast<int>(adjacent.y);
            const int az = static_cast<int>(adjacent.z);
            const InventorySlot slot = player_->Inventory()[static_cast<std::size_t>(player_->Selected())];
            if (slot.type != BlockType::Air && slot.type != BlockType::Stick
                && slot.type != BlockType::Pickaxe && slot.type != BlockType::Axe
                && slot.type != BlockType::Shovel && slot.type != BlockType::Sword
                && world_[static_cast<std::size_t>(Index(ax, ay, az))] == BlockType::Air) {
                world_[static_cast<std::size_t>(Index(ax, ay, az))] = slot.type;
                player_->Remove(slot.type);
            }
        }
    }

    const Vector3 player_position = player_->GetPosition();
    for (auto drop = drops_.begin(); drop != drops_.end();) {
        drop->lifetime -= delta_time;
        drop->vertical_velocity -= MiningConfig::DROP_GRAVITY * delta_time;
        drop->position.y += drop->vertical_velocity * delta_time;
        const int dx = static_cast<int>(std::floor(drop->position.x + 0.5F));
        const int dz = static_cast<int>(std::floor(drop->position.z + 0.5F));
        float support = 0.0F;
        if (dx >= 0 && dx < WorldConfig::WORLD_SIZE && dz >= 0 && dz < WorldConfig::WORLD_SIZE) {
            for (int by = WorldConfig::WORLD_HEIGHT - 1; by >= 0; --by) {
                if (world_[static_cast<std::size_t>(Index(dx, by, dz))] != BlockType::Air && by + 0.5F <= drop->position.y) { support = by + 0.5F; break; }
            }
        }
        if (drop->position.y <= support + MiningConfig::DROP_SIZE * 0.5F) {
            drop->position.y = support + MiningConfig::DROP_SIZE * 0.5F;
            drop->vertical_velocity = 0.0F;
        }
        if (Math3D::Distance(player_position, drop->position) < MiningConfig::PICKUP_RADIUS) {
            player_->Add(drop->type);
            drop = drops_.erase(drop);
        } else if (drop->lifetime <= 0.0F) {
            drop = drops_.erase(drop);
        } else {
            ++drop;
        }
    }
}

void Game::Draw() const {
    ClearBackground(SKYBLUE);
    renderer_->DrawWorld(world_, *player_, drops_, pigs_);
    GuiRenderer().Draw(*player_, inventory_open_);
    if (crafting_open_) {
        DrawRectangle(GameConfig::SCREEN_WIDTH / 2 - 290, 190, 580, 360, Color{66, 53, 43, 245});
        DrawRectangleLines(GameConfig::SCREEN_WIDTH / 2 - 290, 190, 580, 360, RAYWHITE);
        DrawText("CRAFTING TABLE", GameConfig::SCREEN_WIDTH / 2 - 135, 210, 32, WHITE);
        DrawText("K  Sticks: 2 Planks", GameConfig::SCREEN_WIDTH / 2 - 230, 270, 22, RAYWHITE);
        DrawText("P  Pickaxe: 3 Planks + 2 Sticks", GameConfig::SCREEN_WIDTH / 2 - 230, 310, 22, RAYWHITE);
        DrawText("A  Axe: 3 Planks + 2 Sticks", GameConfig::SCREEN_WIDTH / 2 - 230, 350, 22, RAYWHITE);
        DrawText("H  Shovel: 1 Plank + 2 Sticks", GameConfig::SCREEN_WIDTH / 2 - 230, 390, 22, RAYWHITE);
        DrawText("S  Sword: 2 Planks + 1 Stick", GameConfig::SCREEN_WIDTH / 2 - 230, 430, 22, RAYWHITE);
        DrawText("Escape to close", GameConfig::SCREEN_WIDTH / 2 - 75, 500, 18, LIGHTGRAY);
    }
    if (mining_active_) {
        const int x = GameConfig::SCREEN_WIDTH / 2 - 90;
        const int y = GameConfig::SCREEN_HEIGHT / 2 + 24;
        const float progress = std::clamp(mining_elapsed_ / MiningDuration(
            world_[static_cast<std::size_t>(Index(static_cast<int>(mining_target_.x),
                                                   static_cast<int>(mining_target_.y),
                                                   static_cast<int>(mining_target_.z)))]), 0.0F, 1.0F);
        DrawRectangle(x, y, 180, 12, Color{0, 0, 0, 180});
        DrawRectangle(x + 2, y + 2, static_cast<int>(176.0F * progress), 8, GOLD);
    }
}