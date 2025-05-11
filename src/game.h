#pragma once

constexpr Vec2i WORLD_SIZE = {360, 180};
constexpr int TILESIZE = 8;
constexpr Vec2i WORLD_GRID = {WORLD_SIZE.x / TILESIZE,WORLD_SIZE.y / TILESIZE};


struct Tile {
    int neighborMask;
    bool visible;
};

struct GameState {
    bool initialized = false;
    Vec2 playerPos = {0,0};

    Tile worldGrid[WORLD_SIZE.x][WORLD_SIZE.y];
};

static GameState gameState;