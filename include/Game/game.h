#pragma once
#include <memory>
#include <vector>
#include "Entity/Object/Player/player.h"
#include "Entity/Object/Item/item_drop.h"
#include "Entity/animals/pig.h"
#include "Game/game_renderer.h"
class Game{public:Game();void Update(float);void Draw()const;bool Running()const{return running_;}private:int Index(int,int,int)const;void Generate();bool Craft(BlockType,std::initializer_list<std::pair<BlockType,int>>);std::unique_ptr<GameRenderer>renderer_;std::unique_ptr<Player>player_;std::vector<std::unique_ptr<Entity>>entities_;std::vector<BlockType>world_;std::vector<ItemDrop>drops_;std::vector<Pig>pigs_;bool running_{true},inventory_open_{false},crafting_open_{false};};
