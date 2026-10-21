#include "GUI/gui_renderer.h"

#include <string>

#include "Config/game_config.h"
#include "Config/player_config.h"
#include "raylib.h"

void GuiRenderer::Draw(const Player& player, bool inventory_open) const {
    const auto& inventory = player.Inventory();
    const int slot_count = PlayerConfig::INVENTORY_SLOTS;
    const int start_x = GameConfig::SCREEN_WIDTH / 2 - slot_count * 25;
    const int hotbar_y = GameConfig::SCREEN_HEIGHT - 74;

    for (int slot_index = 0; slot_index < slot_count; ++slot_index) {
        const int x = start_x + slot_index * 50;
        DrawRectangle(x, hotbar_y, 48, 48,
                      slot_index == player.Selected() ? GOLD : Color{20, 25, 30, 190});
        DrawRectangleLines(x, hotbar_y, 48, 48, RAYWHITE);
        const InventorySlot& slot = inventory[static_cast<std::size_t>(slot_index)];
        if (slot.type != BlockType::Air) {
            DrawRectangle(x + 9, hotbar_y + 9, 29, 29, BlockColor(slot.type));
            DrawText(std::to_string(slot.count).c_str(), x + 31, hotbar_y + 31, 14, WHITE);
        }
    }

    for (int heart = 0; heart < PlayerConfig::MAX_HEALTH; ++heart) {
        const int x = 22 + heart * 25;
        const Color color = heart < player.Health() ? Color{210,42,48,255} : Color{65,45,48,220};
        DrawRectangle(x+3,20,7,6,color); DrawRectangle(x+13,20,7,6,color);
        DrawRectangle(x,25,23,8,color); DrawRectangle(x+3,33,17,5,color); DrawRectangle(x+7,38,9,4,color);
    }
    DrawRectangle(GameConfig::SCREEN_WIDTH-110,GameConfig::SCREEN_HEIGHT-118,64,96,Color{189,139,92,255});
    DrawRectangle(GameConfig::SCREEN_WIDTH-110,GameConfig::SCREEN_HEIGHT-118,64,18,Color{224,180,125,255});
    DrawText("WASD move | Space jump | Shift sprint | LMB mine | RMB place/use | E inventory",
             20, 53, 18, WHITE);
    DrawCircleLines(GameConfig::SCREEN_WIDTH / 2, GameConfig::SCREEN_HEIGHT / 2, 6, WHITE);

    if (!inventory_open) {
        return;
    }

    DrawRectangle(0, 0, GameConfig::SCREEN_WIDTH, GameConfig::SCREEN_HEIGHT, Color{0, 0, 0, 160});
    DrawText("INVENTORY - 2x2 crafting", GameConfig::SCREEN_WIDTH / 2 - 180, 195, 30, WHITE);
    for (int slot_index = 0; slot_index < slot_count; ++slot_index) {
        const int x = GameConfig::SCREEN_WIDTH / 2 - 240 + (slot_index % 5) * 96;
        const int y = 250 + (slot_index / 5) * 82;
        DrawRectangle(x, y, 82, 70, Color{48, 54, 58, 240});
        DrawRectangleLines(x, y, 82, 70, RAYWHITE);
        const InventorySlot& slot = inventory[static_cast<std::size_t>(slot_index)];
        if (slot.type != BlockType::Air) {
            DrawRectangle(x + 8, y + 8, 25, 25, BlockColor(slot.type));
            DrawText(TextFormat("%s x%d", BlockName(slot.type), slot.count), x + 5, y + 42, 12, WHITE);
        }
    }
    DrawText("C: 1 Log -> 4 Planks | K: 2 Planks -> 4 Sticks | V: 4 Planks -> Table",
             GameConfig::SCREEN_WIDTH / 2 - 300, 430, 18, RAYWHITE);
    DrawText("Open a Crafting Table with RMB to make tools. E or Esc closes inventory.",
             GameConfig::SCREEN_WIDTH / 2 - 310, 462, 18, RAYWHITE);
}