#include "window.h"

Window::Window(int w, int h, const char *name) {
    this->w = w;
    this->h = h;
    this->name = name;
}

Window::~Window() { CloseWindow(); }

void Window::start() {
    InitWindow(this->w, this->h, this->name);
    while (!WindowShouldClose()) {
        BeginDrawing();
        EndDrawing();
    }
}
