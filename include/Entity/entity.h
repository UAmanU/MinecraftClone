#pragma once
#include "raylib.h"
class Entity{public:explicit Entity(Vector3 p):position_(p){}virtual ~Entity()=default;virtual void Update(float)=0;virtual void Draw()const=0;Vector3 GetPosition()const{return position_;}protected:Vector3 position_{};};
