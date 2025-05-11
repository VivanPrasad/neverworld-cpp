#include "neverlib.h"
#include "assets.h"
#include "render_interface.h"
#include "game.h"

// Game Constances
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;

constexpr int TILESIZE = 8;
// Game Structs

// Game Functions

void init() {
    renderData.gameCamera.dimensions = {WORLD_WIDTH,WORLD_HEIGHT};
}

void process() {
    if (is_key_down(KEY_W)) {renderData.gameCamera.position.y -= 1.0f;}
    if (is_key_down(KEY_A)) {renderData.gameCamera.position.x += 1.0f;}
    if (is_key_down(KEY_S)) {renderData.gameCamera.position.y += 1.0f;}
    if (is_key_down(KEY_D)) {renderData.gameCamera.position.x -= 1.0f;}
}

void update() {
    draw_sprite(SPRITE_PLAYER, {0.0f, 0.0f}, {8.0f, 16.0f});
    process();
}