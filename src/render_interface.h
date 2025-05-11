#pragma once

// Renderer Constants
constexpr int MAX_TRANSFORMS = 1000;
// Renderer Stucts
struct Transform {
    Vec2 pos;
    Vec2 size;
    Vec2i atlasOffset;
    Vec2i spriteSize;
};

struct RenderData {
    int transformCount = 0;
    Transform transforms[MAX_TRANSFORMS];
};
// Renderer Global Variables
static RenderData renderData;

// Renderer Functions

void draw_sprite(SpriteID spriteID, Vec2 pos, Vec2 size) {
    Sprite sprite = getSprite(spriteID);
    Transform transform = {};
    transform.pos = pos;
    transform.size = size;
    transform.atlasOffset = sprite.atlasOffset;
    transform.spriteSize = sprite.spriteSize;
    
    // Assume that transforms array is never full
    renderData.transforms[renderData.transformCount++] = transform;
}