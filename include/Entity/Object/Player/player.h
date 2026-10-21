#pragma once
#include <array>
#include <algorithm>
#include "Config/block_config.h"
#include "Config/player_config.h"
#include "Entity/entity.h"
struct InventorySlot{BlockType type{BlockType::Air};int count{0};};
class Player:public Entity{public:explicit Player(Vector3);void Update(float)override;void Draw()const override;Camera3D& Camera(){return camera_;}const Camera3D& Camera()const{return camera_;}const auto& Inventory()const{return inventory_;}int Selected()const{return selected_;}void Select(int n){if(n>=0&&n<PlayerConfig::INVENTORY_SLOTS)selected_=n;}int Health()const{return health_;}void Damage(int n){health_=std::max(0,health_-n);}void Add(BlockType,int=1);bool Remove(BlockType,int=1);int Count(BlockType)const;private:std::array<InventorySlot,PlayerConfig::INVENTORY_SLOTS>inventory_{};Camera3D camera_{};int selected_{0},health_{PlayerConfig::MAX_HEALTH};};
