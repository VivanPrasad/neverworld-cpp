#pragma once

#define is_down(b) input -> buttons[b].is_down
#define pressed(b) (is_down(b) && input->buttons[b].changed)
#define released(b) (!is_down(b) && input->buttons[b].changed)
#define handle_input(b, vk) \
    case vk: {\
        input.buttons[b].is_down = is_down;\
        input.buttons[b].changed = true;\
    } break;

struct ButtonState {
    bool is_down, changed;
};

enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_COUNT, // Should be last item
};

struct Input {
    ButtonState buttons[BUTTON_COUNT];
    int screenWidth = 1280, screenHeight = 720;
};

static Input input = {};
