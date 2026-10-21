#include "Menu/menu_renderer.h"
#include "raylib.h"
void MenuRenderer::Draw(bool h)const{ClearBackground(SKYBLUE);const char*t="AICRAFT";DrawText(t,640-MeasureText(t,86)/2,180,86,WHITE);DrawText("A tiny block-building adventure",446,285,24,RAYWHITE);Rectangle b{515,375,250,66};DrawRectangleRounded(b,.18F,8,h?GREEN:DARKGREEN);DrawText("START GAME",566,395,25,WHITE);}
