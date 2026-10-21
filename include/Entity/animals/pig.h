#pragma once
#include "Entity/entity.h"
class Pig:public Entity{public:explicit Pig(Vector3 p):Entity(p){}void Update(float)override{}void Draw()const override;};
