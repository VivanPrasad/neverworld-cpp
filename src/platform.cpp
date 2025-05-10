#pragma once
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>
#include "string"

struct RenderState {
    int width;
    int height;
    void* memory;
    BITMAPINFO bitmap_info;
};

struct Performance {
    float delta = 0.016666f;
    float frequency;
    LARGE_INTEGER frame_begin;
    LARGE_INTEGER frame_end;
};

static RenderState render_state;
static Performance performance;

void resize_window_buffer(HWND *window_handle) {
    RECT rect;
    GetClientRect(*window_handle, &rect);
    render_state.width = rect.right - rect.left;
    render_state.height = rect.bottom - rect.top;

    int size = render_state.width * render_state.height * sizeof(u32);
    if (render_state.memory) VirtualFree(render_state.memory, 0 , MEM_RELEASE);
    render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
    render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
    render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
    render_state.bitmap_info.bmiHeader.biPlanes = 1;
    render_state.bitmap_info.bmiHeader.biBitCount = 32;
    render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch(uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: { running = false; } break;
        case WM_SIZE: { resize_window_buffer(&hwnd); } break;
        default: { result = DefWindowProc(hwnd,uMsg,wParam,lParam); }
    }
    return result;
}

HWND platform_create(int width, int height) {
    HINSTANCE hInstance = GetModuleHandleA(0);
    WNDCLASS window_class = {};

    LPCSTR title = (LPCSTR)TEXT("NEVERWORLD");
    int dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.hIcon = LoadIcon(0, IDI_APPLICATION);
    window_class.hCursor = LoadCursor(0, IDC_ARROW);

    window_class.lpszClassName = TEXT("NEVERWORLD");
    window_class.lpfnWndProc = &window_callback;
    window_class.hInstance = hInstance;
    
    // Register Class
    RegisterClass(&window_class);

    // Create Window
    HWND window = CreateWindowExA(
        0,title,title,dwStyle,100,100,width,height,NULL,NULL,hInstance,NULL);

    ShowWindow(window, SW_SHOW);

    return window;
}

static void handle_window_input(HWND window_handle) {
    MSG message;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        input.buttons[i].changed = false;
    }
    while (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE)) {
        switch(message.message) {
            case WM_KEYUP:
            case WM_KEYDOWN: {
                u32 vk_code = (u32)message.wParam;
                bool is_down = ((message.lParam & (1 << 31)) == 0);

                switch (vk_code) {
                    handle_input(BUTTON_UP, VK_UP);
                    handle_input(BUTTON_DOWN, VK_DOWN);
                    handle_input(BUTTON_LEFT, VK_LEFT);
                    handle_input(BUTTON_RIGHT, VK_RIGHT);
                }
            } break;
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message);
            } break;
        }
    }
}

static void setup_performance() {
    LARGE_INTEGER perf;
    QueryPerformanceFrequency(&perf);
    performance.frequency = (float)perf.QuadPart;
    QueryPerformanceCounter(&performance.frame_begin);
}

static void update_frame_time() {
    QueryPerformanceCounter(&performance.frame_end);
    performance.delta = (float)(performance.frame_end.QuadPart \
        - performance.frame_begin.QuadPart) / performance.frequency;
    performance.frame_begin = performance.frame_end;
}
#endif