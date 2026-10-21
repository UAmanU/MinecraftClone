#include "Menu/menu.h"
#include "raylib.h"
Menu::Menu():renderer_(std::make_unique<MenuRenderer>()){}void Menu::Update(bool&s){Rectangle b{515,375,250,66};if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&CheckCollisionPointRec(GetMousePosition(),b))||IsKeyPressed(KEY_ENTER))s=true;}void Menu::Draw()const{Rectangle b{515,375,250,66};renderer_->Draw(CheckCollisionPointRec(GetMousePosition(),b));}
