#include <windows.h>
#include "wglext.h"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif

struct Performance {
    float delta = 0.016666f;
    float frequency;
    LARGE_INTEGER frame_begin;
    LARGE_INTEGER frame_end;
};

static Performance performance;
static HWND window;
static HDC dc;

void resize_window() {
    RECT rect;
    GetClientRect(window, &rect);
    input.screenWidth = rect.right - rect.left;
    input.screenHeight = rect.bottom - rect.top;
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch(uMsg) {
        case WM_CLOSE: {running = false;} break;
        case WM_SIZE: {resize_window();} break;
        default: {result = DefWindowProc(hwnd,uMsg,wParam,lParam);}
    }
    return result;
}

void platform_create_window(int width, int height) {
    HINSTANCE hInstance = GetModuleHandleA(0);
    WNDCLASSA wc = {};

    LPCSTR title = (LPCSTR)TEXT("NEVERWORLD");
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = &window_callback;
    
    // Register Class
    SM_ASSERT(RegisterClassA(&wc), "Failed to register window class");
    int dwStyle = WS_OVERLAPPEDWINDOW;
    // OpenGL Fake Device Context
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    {
        // Create Test Window
        window = CreateWindowExA(
            0,title,title,dwStyle,100,100,width,height,NULL,NULL,hInstance,NULL);
        SM_ASSERT(window, "Failed to create fake window");

        HDC fakeDC = GetDC(window);
        SM_ASSERT(fakeDC, "Failed to create fake device context");
    
        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //Interleaves images
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;

        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);
        SM_ASSERT(pixelFormat, "Failed to choose DC pixel format");
        SM_ASSERT(SetPixelFormat(fakeDC, pixelFormat, &pfd),"Failed to set pixel format");
        
        HGLRC fakeRC = wglCreateContext(fakeDC);
        SM_ASSERT(fakeRC, "Failed to create OpenGL rendering context");
        SM_ASSERT(wglMakeCurrent(fakeDC, fakeRC), "Failed to make OpenGL context current");

        wglChoosePixelFormatARB = \
            (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = \
            (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        SM_ASSERT(wglCreateContextAttribsARB && wglChoosePixelFormatARB, 
            "Failed to load OpenGL functions");
        
        wglMakeCurrent(fakeDC, 0);
        wglDeleteContext(fakeRC);
        ReleaseDC(window, fakeDC);
        
        DestroyWindow(window);
    }
    // Actual OpenGL initialization
    {
        RECT borderRect = {};
        AdjustWindowRectEx(&borderRect, dwStyle, 0, 0);

        width += borderRect.right - borderRect.left;
        height += borderRect.bottom - borderRect.top;

        // Create Window Again
        window = CreateWindowExA(
            0,title,title,dwStyle,100,100,width,height,NULL,NULL,hInstance,NULL);
        SM_ASSERT(window, "Failed to create window");

        dc = GetDC(window);
        SM_ASSERT(dc, "Failed to create temp device context");
        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            0 // Terminate with 0, or OpenGL throws an error
        };

        UINT numPixelFormats;
        int pixelFormat = 0;
        SM_ASSERT(wglChoosePixelFormatARB(dc, pixelAttribs, 0, 1, &pixelFormat, &numPixelFormats), 
            "Failed to choose wglChoosePixelFormatARB");
        
        PIXELFORMATDESCRIPTOR pfd = {0};
        DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        SM_ASSERT(SetPixelFormat(dc,pixelFormat,&pfd), "Failed to SetPixelFormat");

        const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0 // Terminate with 0, or OpenGL throws an error
        };

        HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
        SM_ASSERT(rc, "Failed to create OpenGL rendering context");
        SM_ASSERT(wglMakeCurrent(dc, rc), "Failed to make OpenGL context current");
    }

    ShowWindow(window, SW_SHOW);
}

static void handle_window_input() {
    MSG message;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        input.buttons[i].changed = false;
    }
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
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

void* platform_load_gl_function(char* funName) {
    PROC proc = wglGetProcAddress(funName);
    if (!proc) {
        static HMODULE openglDLL = LoadLibraryA("opengl32.dll");
        proc = GetProcAddress(openglDLL, funName);
    }
    SM_ASSERT(proc, "Failed to load OpenGL function: %s", funName);
    return (void*)proc;

}

void platform_swap_buffers() {
    SwapBuffers(dc);
}