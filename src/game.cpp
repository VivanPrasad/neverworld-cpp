#include "neverlib.h"
#include "assets.h"
#include "render_interface.h"

// Game Constances

// Game Structs

// Game Functions

void update_game() {
    draw_sprite(SPRITE_PLAYER, {100.0f, 100.0f}, {100.0f, 100.0f});
    printf("Game updated\n");
}