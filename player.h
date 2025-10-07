#pragma once

#include "obj.h"
#include <cstdio>

class Player : public Obj {
    public:
        Vector2 accel = {0, 0};
        Vector2 velocity = {0, 0};
        float max_speed = 200.0f;
        float accel_rate = 900.0f;
        float friction = 800.0f;

        Player(int id) : Obj(id) {}
        void draw() { DrawRectangleRec(this->rec, this->col); }
};
