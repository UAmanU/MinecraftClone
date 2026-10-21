#include "Menu/menu_renderer.h"
#include "Config/game_config.h"
#include "raylib.h"
void MenuRenderer::Draw(bool start_hovered,bool quit_hovered)const{
    ClearBackground(SKYBLUE);
    const char* title="AICRAFT";
    DrawText(title,GameConfig::SCREEN_WIDTH/2-MeasureText(title,86)/2,GameConfig::SCREEN_HEIGHT/2-195,86,WHITE);
    DrawText("A tiny block-building adventure",GameConfig::SCREEN_WIDTH/2-194,GameConfig::SCREEN_HEIGHT/2-90,24,RAYWHITE);
    const Rectangle start{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2-10.0F,250,66};
    DrawRectangleRounded(start,.18F,8,start_hovered?GREEN:DARKGREEN);
    DrawText("START GAME",GameConfig::SCREEN_WIDTH/2-74,GameConfig::SCREEN_HEIGHT/2+10,25,WHITE);
    const Rectangle quit{GameConfig::SCREEN_WIDTH/2-125.0F,GameConfig::SCREEN_HEIGHT/2+80.0F,250,58};
    DrawRectangleRounded(quit,.18F,8,quit_hovered?MAROON:Color{115,45,45,255});
    DrawText("EXIT",GameConfig::SCREEN_WIDTH/2-25,GameConfig::SCREEN_HEIGHT/2+97,24,WHITE);
}