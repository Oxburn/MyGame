#include "raylib.h"
#include "Window.h"

int main(void) {
    Window();
    return 0;
}

/*
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/msys64/mingw64"
cmake --build build 
.\build\raylib_game.exe
*/