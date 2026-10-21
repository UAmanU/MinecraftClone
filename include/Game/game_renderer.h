#pragma once
#include "Game/world.h"
#include "Entity/Object/Item/item_drop.h"
#include "Entity/animals/pig.h"
class Player;
class GameRenderer {
public:
    void DrawFrame(const World&, const Player&, const std::vector<ItemDrop>&, const std::vector<Pig>&,
                   bool inventory_open, bool crafting_open, float mining_progress) const;
    bool Raycast(const World&, const Camera3D&, Vector3& hit, Vector3& adjacent) const;
private:
    void DrawWorld(const World&, const Player&, const std::vector<ItemDrop>&, const std::vector<Pig>&) const;
    void DrawCraftingMenu() const;
    void DrawMiningProgress(float progress) const;
};
