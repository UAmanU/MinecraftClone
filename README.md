# MinecraftCodexClone

A small first-person voxel sandbox made with C++20 and raylib.

## Build (Windows / MinGW)

The project expects raylib in the sibling folder `../Libraries/raylib_files`.

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/MinecraftCodexClone.exe
```

Controls: WASD move, mouse look, Space jump, Shift sprint, left mouse mine, right mouse place or use a crafting table, E inventory, number keys select a hotbar slot, Escape returns to the menu. Use Exit or Escape from the main menu to close the game.

In inventory, press C to turn a log into 4 planks, K to craft 4 sticks from 2 planks, and V to craft a table from 4 planks. At a crafting table, K makes sticks, P a pickaxe, A an axe, H a shovel, and S a sword. Mining takes time; axes speed up logs and shovels speed up dirt and grass.

The world is generated locally each run. Saving/loading is not implemented.
