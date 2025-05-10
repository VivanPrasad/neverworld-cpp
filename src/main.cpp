#include "iostream"
#include "utils.cpp"
#include "input.cpp"

static bool running = true;
static Input input = {};

#define handle_input(b, vk) \
    case vk: {\
        input.buttons[b].is_down = is_down;\
        input.buttons[b].changed = true;\
    } break;

#include "platform.cpp"
#include "neverlib.h"
#include "renderer.cpp"
#include "game.cpp"

int main() {
    HWND window = platform_create(1280, 720);
    HDC hdc = GetDC(window);
    setup_performance();

    while (running) {
        // Input
        handle_window_input(window);

        // Simulate
        simulate_game(&input, performance.delta);

        // Render
        StretchDIBits(
            hdc, 0, 0, render_state.width, render_state.height, 0, 0,
            render_state.width, render_state.height, render_state.memory, 
            &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
        
        //SM_TRACE("Trace");
        //SM_WARN("Warn");
        //SM_ERROR("Error");
        //SM_ASSERT(false,"Invalid window");
        update_frame_time();
    }
    return EXIT_SUCCESS;
}