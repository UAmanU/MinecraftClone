#include "Menu/menu.h"
#include "Config/game_config.h"
#include "raylib.h"
Menu::Menu():renderer_(std::make_unique<MenuRenderer>()){}
void Menu::Update(bool& start,bool& quit){
    const Rectangle start_button{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2-10.0F,250,66};
    const Rectangle quit_button{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2+80.0F,250,58};
    const Vector2 mouse=GetMousePosition();
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(CheckCollisionPointRec(mouse,start_button))start=true;
        if(CheckCollisionPointRec(mouse,quit_button))quit=true;
    }
    if(IsKeyPressed(KEY_ENTER))start=true;
    if(IsKeyPressed(KEY_ESCAPE))quit=true;
}
void Menu::Draw()const{
    const Vector2 mouse=GetMousePosition();
    renderer_->Draw(CheckCollisionPointRec(mouse,{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2-10.0F,250,66}),CheckCollisionPointRec(mouse,{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2+80.0F,250,58}));
}