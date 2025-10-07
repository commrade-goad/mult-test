#include "game.h"
#include "player.h"
#include "window.h"
#include "id.h"
#include "raylib5.5/include/raylib.h"
#include "raylib5.5/include/raymath.h"

#include <cstdlib>
#include <ctime>

Game::Game(void *w) {
    this->w = w;
    srand(time(nullptr));

    std::vector<std::pair<int,int>> emptyCells;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (!map[i][j]) emptyCells.emplace_back(i, j);
        }
    }

    auto pptr = std::make_shared<Player>(CONTROLLABLE_PLAYER);
    pptr->col = GetColor(0x1e1e2eff);

    if (!emptyCells.empty()) {
        auto [i, j] = emptyCells[rand() % emptyCells.size()];
        Window *win = (Window *) this->w;
        int cell_size = tile_size;
        int map_pixel_width  = cell_size * MAP_WIDTH;
        int map_pixel_height = cell_size * MAP_HEIGHT;
        int offset_x = (win->w - map_pixel_width)  / 2;
        int offset_y = (win->h - map_pixel_height) / 2;

        float x = offset_x + j * cell_size;
        float y = offset_y + i * cell_size;
        pptr->rec = {x, y, (float)cell_size, (float)cell_size};
    }

    obs.push_back(pptr);
}

void Game::logic(float dt) {
    for (auto &o : obs) {
        if (o->id == CONTROLLABLE_PLAYER && this->p1 == nullptr)
            this->p1 = std::dynamic_pointer_cast<Player>(o);
        o->logic(dt);
    }

    if (!p1) return;

    Vector2 accel = {0, 0};
    if (IsKeyDown(KEY_W)) accel.y -= 1.0f;
    if (IsKeyDown(KEY_S)) accel.y += 1.0f;
    if (IsKeyDown(KEY_A)) accel.x -= 1.0f;
    if (IsKeyDown(KEY_D)) accel.x += 1.0f;

    if (accel.x != 0 || accel.y != 0)
        accel = Vector2Normalize(accel);

    p1->velocity.x += accel.x * p1->accel_rate * dt;
    p1->velocity.y += accel.y * p1->accel_rate * dt;

    float speed = Vector2Length(p1->velocity);
    if (speed > p1->max_speed) {
        p1->velocity = Vector2Scale(Vector2Normalize(p1->velocity), p1->max_speed);
    }

    if (accel.x == 0 && accel.y == 0) {
        float decel = p1->friction * dt;
        float curSpeed = Vector2Length(p1->velocity);
        if (curSpeed > decel)
            p1->velocity = Vector2Scale(Vector2Normalize(p1->velocity), curSpeed - decel);
        else
            p1->velocity = {0, 0};
    }

    Vector2 old_pos = {
        p1->rec.x,
        p1->rec.y,
    };

    p1->rec.x += p1->velocity.x * dt;
    p1->rec.y += p1->velocity.y * dt;

    // Get window information to adjust tile calculations
    Window *win = (Window *)this->w;
    int map_pixel_width = tile_size * MAP_WIDTH;
    int map_pixel_height = tile_size * MAP_HEIGHT;
    int offset_x = (win->w - map_pixel_width) / 2;
    int offset_y = (win->h - map_pixel_height) / 2;

    // Convert player position to map coordinates
    int playerTileX = (int)((p1->rec.x - offset_x) / this->tile_size);
    int playerTileY = (int)((p1->rec.y - offset_y) / this->tile_size);
    bool collided = false;

    // Check for collision only with nearby tiles (in a 3x3 grid around player)
    const int CHECK_RADIUS = 1; // Check 1 tile in each direction

    for (int y = playerTileY - CHECK_RADIUS; y <= playerTileY + CHECK_RADIUS; y++) {
        if (y < 0 || y >= this->MAP_HEIGHT) continue; // Skip if out of map bounds

        for (int x = playerTileX - CHECK_RADIUS; x <= playerTileX + CHECK_RADIUS; x++) {
            if (x < 0 || x >= this->MAP_WIDTH) continue; // Skip if out of map bounds

            // Check if this tile is solid/collidable
            if (this->map[y][x]) { // 1 = solid, 0 = empty
                // Create rectangle for this tile with proper offset
                Rectangle tileRect = {
                    (float)(offset_x + x * this->tile_size),
                    (float)(offset_y + y * this->tile_size),
                    (float)this->tile_size,
                    (float)this->tile_size
                };

                // Check collision between player and this tile
                if (CheckCollisionRecs(p1->rec, tileRect)) {
                    collided = true;
                    break;
                }
            }
        }

        if (collided) break;
    }

    if (collided) {
        p1->rec.x = old_pos.x;
        p1->rec.y = old_pos.y;
        p1->velocity = {0, 0};
    }
}

void Game::draw() {
    ClearBackground(BLACK);
    this->_draw_map();
    for (int i = 0; i < this->obs.size(); i++) { obs[i]->draw(); }
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
            if (this->map[i][j]) tileColor = GetColor(0xaeaeae);

            int x = offset_x + j * cell_size;
            int y = offset_y + i * cell_size;

            DrawRectangle(x, y, tile_size, tile_size, tileColor);
        }
    }
}
