#pragma once

#include "raylib5.5/include/raylib.h"
#include "game.h"

class Window {
    public:
        int w, h;
        const char *name;
        Game g;

        Window(int w, int h, const char *name);
        ~Window();
        void start();
};
