#pragma once

#include "raylib5.5/include/raylib.h"
class Window {
    public:
        int w, h;
        const char *name;

        Window(int w, int h, const char *name);
        ~Window();
        void start();
};
