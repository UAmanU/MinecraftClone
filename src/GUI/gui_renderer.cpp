#include "GUI/gui_renderer.h"
#include <string>
#include "raylib.h"
void GuiRenderer::Draw(const Player&p,bool open)const{auto&i=p.Inventory();for(int n=0;n<10;++n){int x=390+n*50,y=646;DrawRectangle(x,y,48,48,n==p.Selected()?GOLD:Color{20,25,30,190});DrawRectangleLines(x,y,48,48,RAYWHITE);if(i[(size_t)n].type!=BlockType::Air){DrawRectangle(x+9,y+9,29,29,BlockColor(i[(size_t)n].type));DrawText(std::to_string(i[(size_t)n].count).c_str(),x+31,y+31,14,WHITE);}}DrawText(TextFormat("HP %d / 10",p.Health()),20,20,24,WHITE);DrawText("WASD move | Space jump | Shift sprint | LMB mine | RMB place | E inventory",20,53,18,WHITE);DrawCircleLines(640,360,6,WHITE);if(open){DrawRectangle(0,0,1280,720,Color{0,0,0,160});DrawText("INVENTORY - C: Log to Planks | V: Crafting Table",390,230,25,WHITE);}}
