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
    input.screenSize = {1280,720};
    BumpAllocator transientStorage = make_bump_allocator(MB(10));
    platform_create_window();
    platform_fill_keycodes();
    gl_init(&transientStorage);
    
    start_frame_time();
    init();
    while (running) {
        // Handle Input
        platform_update_window();

        // Update Game
        update();

        // Render
        gl_render(&transientStorage);
        platform_swap_buffers();
        
        // Reset Performance
        reset_frame_time();
    }
    return EXIT_SUCCESS;
}
