#pragma once
#include <memory>
#include "Game/game.h"
#include "Menu/menu.h"
class ProgrammLauncher{public:ProgrammLauncher();~ProgrammLauncher();void Run();private:std::unique_ptr<Game>game_;std::unique_ptr<Menu>menu_;bool in_game_{false};};
