#pragma once

// ################################ Renderer Constants ################################
constexpr int MAX_TRANSFORMS = 1000;

// ################################ Renderer Stucts ################################

struct Camera2D {
    float zoom = 1.0f;
    Vec2 dimensions;
    Vec2 position;
};

struct Transform {
    Vec2 pos;
    Vec2 size;
    Vec2i atlasOffset;
    Vec2i spriteSize;
};

struct RenderData {
    Camera2D gameCamera;
    Camera2D uiCamera; // Will be used for UI

    int transformCount = 0;
    Transform transforms[MAX_TRANSFORMS];
};
// ############################ Renderer Global Variables ################################
static RenderData renderData;
// ################################ Renderer Utility ################################

Vec2i screen_to_world(Vec2i screenPos) {
    Camera2D camera = renderData.gameCamera;
    int worldX = screenPos.x / input.screenSize.x * camera.dimensions.x;
    worldX += -camera.dimensions.x * 0.5f + camera.position.x;
    int worldY = screenPos.y / input.screenSize.y * camera.dimensions.y;
    worldY += -camera.dimensions.y * 0.5f + camera.position.y;
    return {worldX, worldY};
}
// ################################ Renderer Functions ################################

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