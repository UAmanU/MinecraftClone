#pragma once
#include <vector>
#include "Config/block_config.h"
#include "Entity/Object/Item/item_drop.h"
#include "Entity/animals/pig.h"
class Player;
class GameRenderer{public:void DrawWorld(const std::vector<BlockType>&,const Player&,const std::vector<ItemDrop>&,const std::vector<Pig>&)const;bool Raycast(const std::vector<BlockType>&,const Camera3D&,Vector3&,Vector3&)const;};
