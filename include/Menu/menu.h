#pragma once
#include <memory>
#include "Menu/menu_renderer.h"
class Menu{public:Menu();void Update(bool&,bool&);void Draw()const;private:std::unique_ptr<MenuRenderer>renderer_;};
