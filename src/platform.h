#pragma once
static bool running = true;

void* platform_load_gl_function(char* funName);
void platform_swap_buffers();
void platform_fill_keycodes();
void platform_create_window();