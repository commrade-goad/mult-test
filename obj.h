#pragma once

#include "raylib5.5/include/raylib.h"

class Obj {
    public:
        int id;
        Rectangle rec;
        Color col;
        Texture txt;

        Obj(int id) { this->id = id; };
        Obj(int id, Rectangle rec, Color col, Texture txt) : id(id), rec(rec), col(col), txt(txt) {}
        virtual ~Obj() = default;
        void virtual draw() = 0;
        void virtual logic(float dt) {}
};
