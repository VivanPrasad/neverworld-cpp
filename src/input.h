#pragma once
// ################################### STRUCTS ###################################

enum KeyCode {
    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,
    
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_Q, KEY_W, KEY_E, // E key interact + E menu
    KEY_A, KEY_S, KEY_D,
    KEY_Z, KEY_X, KEY_C, // Other quick menus?
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, //Arrow keys or WASD keys
    KEY_LSHIFT, KEY_RSHIFT, // Running

    KEY_F11, // Toggle Fullscreen
    KEY_ESCAPE, // Toggle Pause Menu
    KEY_TAB, // Toggle UI

    KEY_SPACE, // Spamming to do stuff
    KEY_ENTER, // Accept (interact)

    KEY_COUNT = 255
};

struct Key {
    bool down;
    bool changed;
};

struct Input {
    Vec2i screenSize;

    Vec2i prevMousePos;
    Vec2i mousePos;
    Vec2i relativeMousePos;

    Vec2i prevScreenMousePos;
    Vec2i screenMousePos;
    Vec2i relativeScreenMousePos;

    Key keys[KEY_COUNT];
    
};

// ################################### GLOBALS ###################################
static Input input = {};

// ################################## FUNCTIONS ##################################
bool is_key_down(KeyCode key) {
    return input.keys[key].down;
}
bool is_key_pressed(KeyCode key) {
    return (is_key_down(key) && input.keys[key].changed);
}
bool is_key_released(KeyCode key) {
    return (!is_key_down(key) && input.keys[key].changed);
}

void handle_key_event(KeyCode key, bool isDown) {
    input.keys[key].down = isDown; 
    input.keys[key].changed = true; 
}

bool reset_input() {
    for (int i = 0; i < KEY_COUNT; i++) {
        input.keys[i].changed = false;
    }
}

