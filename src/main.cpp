#include "neverlib.h"
#include "input.h"
#include "game.cpp"
#include "platform.h"
#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"
#ifdef _WIN32
#include "win32_platform.cpp"
#endif
#include "gl_renderer.cpp"

int main() {
    input.screenWidth = 1280;
    input.screenHeight = 720;
    BumpAllocator transientStorage = make_bump_allocator(MB(10));
    platform_create_window(input.screenWidth, input.screenHeight);
    gl_init(&transientStorage);
    setup_frame_time();

    while (running) {
        // Input
        handle_window_input();

        // Simulate

        // Render
        update_game();
        gl_render(&transientStorage);

        platform_swap_buffers();
        
        // Time
        update_frame_time();
    }
    return EXIT_SUCCESS;
}
