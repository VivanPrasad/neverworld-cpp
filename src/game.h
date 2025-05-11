#pragma once

struct GameState {
    bool initialized = false;
    Vec2i playerPos;
};

static GameState gameState;