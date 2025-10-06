#include "window.h"

Window::Window(int w, int h, const char *name) {
    this->w = w;
    this->h = h;
    this->name = name;
    this->g = Game();
    this->g.w = this;
}

Window::~Window() { CloseWindow(); }

void Window::start() {
    InitWindow(this->w, this->h, this->name);
    while (!WindowShouldClose()) {
        g.logic(GetFrameTime());

        BeginDrawing();

            g.draw();

        EndDrawing();
    }
}
