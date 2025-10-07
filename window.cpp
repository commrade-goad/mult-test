#include "window.h"

Window::Window(int w, int h, const char *name)
    : w(w), h(h), name(name), g((void *)this)
{}

Window::~Window() { CloseWindow(); }

void Window::start() {
    InitWindow(this->w, this->h, this->name);
    while (!WindowShouldClose() || this->g.exit) {
        g.logic(GetFrameTime());

        BeginDrawing();

            g.draw();

        EndDrawing();
    }
}
