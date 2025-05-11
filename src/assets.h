#pragma once

// Assets Constants
// Assets Structs
enum SpriteID {
    SPRITE_PLAYER,
    SPRITE_COUNT
};

struct Sprite {
    Vec2i atlasOffset;
    Vec2i spriteSize;
};
// Assets Functions
Sprite getSprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch(spriteID) {
        case SPRITE_PLAYER: {
            sprite.atlasOffset = {0, 0};
            sprite.spriteSize = {8, 16};
        } break;
        default: {
            SM_ASSERT(false, "Invalid spriteID: %d", spriteID);
        }
    }
    return sprite;
}