#include "neverlib.h"
#include "assets.h"
#include "render_interface.h"
#include "game.h"

// Game Constances

// Game Structs

// Game Functions

Tile* get_tile(int x, int y) {
    Tile* tile = nullptr;
    if (x >= 0 && x < WORLD_SIZE.x && y >= 0 && y < WORLD_SIZE.y) {
        tile = &gameState.worldGrid[x][y];
    }
    return tile;
}

Tile* get_tile(Vec2i worldPos) {
    return get_tile(worldPos / TILESIZE);
}

// Game Logic
void init() {
    renderData.gameCamera.dimensions = {320, 180};
    renderData.gameCamera.position = {160,-90};
}

void process() {
    if (is_key_down(KEY_W)) {--gameState.playerPos.y;}
    if (is_key_down(KEY_A)) {--gameState.playerPos.x;}
    if (is_key_down(KEY_S)) {++gameState.playerPos.y;}
    if (is_key_down(KEY_D)) {++gameState.playerPos.x;}
}

void update() {
    draw_sprite(SPRITE_PLAYER, gameState.playerPos, {8.0f, 16.0f});
    process();
}