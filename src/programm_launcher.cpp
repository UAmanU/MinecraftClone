#include "programm_launcher.h"
#include "Config/game_config.h"
#include "raylib.h"
ProgrammLauncher::ProgrammLauncher(){InitWindow(GameConfig::SCREEN_WIDTH,GameConfig::SCREEN_HEIGHT,GameConfig::WINDOW_TITLE);SetTargetFPS(GameConfig::GAME_FPS);SetExitKey(KEY_NULL);menu_=std::make_unique<Menu>();}ProgrammLauncher::~ProgrammLauncher(){CloseWindow();}
void ProgrammLauncher::Run(){while(!WindowShouldClose()){if(in_game_){game_->Update(GetFrameTime());if(!game_->Running()){in_game_=false;EnableCursor();menu_=std::make_unique<Menu>();}BeginDrawing();if(in_game_)game_->Draw();else menu_->Draw();EndDrawing();}else{bool start=false,quit=false;menu_->Update(start,quit);if(quit)break;if(start){game_=std::make_unique<Game>();in_game_=true;DisableCursor();}BeginDrawing();menu_->Draw();EndDrawing();}}}
