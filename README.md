# MinecraftCodexClone

A small first-person voxel sandbox made with C++20 and raylib.

## Build (Windows / MinGW)

The project expects raylib in the sibling folder `../Libraries/raylib_files`.

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/MinecraftCodexClone.exe
```

Controls: WASD move, mouse look, Space jump, Shift sprint, left mouse mine, right mouse place or use a crafting table, E inventory, number keys select a hotbar slot, Escape returns to the menu.

The world is generated locally each run. Saving/loading is not implemented.
