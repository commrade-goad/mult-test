#include "game.h"
#include "window.h"
#include "raylib5.5/include/raylib.h"

Game::Game() {
}
void Game::logic(float dt) {}
void Game::draw() {
    ClearBackground(BLACK);
    this->_draw_map();
}

void Game::_draw_map() {
    Window *win = (Window *) this->w;

    int cell_size = tile_size;
    int map_pixel_width  = cell_size * MAP_WIDTH;
    int map_pixel_height = cell_size * MAP_HEIGHT;
    int offset_x = (win->w - map_pixel_width)  / 2;
    int offset_y = (win->h - map_pixel_height) / 2;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            Color tileColor = WHITE;
            if (this->map[i][j]) tileColor = RED;

            int x = offset_x + j * cell_size;
            int y = offset_y + i * cell_size;

            DrawRectangle(x, y, tile_size, tile_size, tileColor);
        }
    }
}
