#pragma once
#include <memory>
#include <vector>
#include "Entity/Object/Player/player.h"
#include "Entity/Object/Item/item_drop.h"
#include "Entity/animals/pig.h"
#include "Game/game_renderer.h"
class Game{public:Game();void Update(float);void Draw()const;bool Running()const{return running_;}private:int Index(int,int,int)const;void Generate();bool Craft(BlockType,std::initializer_list<std::pair<BlockType,int>>);float MiningDuration(BlockType)const;std::unique_ptr<GameRenderer>renderer_;std::unique_ptr<Player>player_;std::vector<std::unique_ptr<Entity>>entities_;std::vector<BlockType>world_;std::vector<ItemDrop>drops_;std::vector<Pig>pigs_;float vertical_velocity_{0.0F},yaw_{-1.57079632679F},pitch_{0.0F},mining_elapsed_{0.0F};Vector3 mining_target_{};bool mining_active_{false};bool running_{true},inventory_open_{false},crafting_open_{false};};
