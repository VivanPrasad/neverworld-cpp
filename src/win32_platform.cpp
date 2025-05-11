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
static KeyCode keyCodes[KEY_COUNT];
static HWND window;
static HDC dc;

static void start_frame_time() {
    LARGE_INTEGER perf;
    QueryPerformanceFrequency(&perf);
    performance.frequency = (float)perf.QuadPart;
    QueryPerformanceCounter(&performance.frame_begin);
}

static void reset_frame_time() {
    QueryPerformanceCounter(&performance.frame_end);
    performance.delta = (float)(performance.frame_end.QuadPart \
        - performance.frame_begin.QuadPart) / performance.frequency;
    performance.frame_begin = performance.frame_end;
}

void resize_window() {
    RECT rect;
    GetClientRect(window, &rect);
    input.screenSize.x = rect.right - rect.left;
    input.screenSize.y = rect.bottom - rect.top;
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch(message) {
        case WM_CLOSE: {running = false;} break;
        case WM_SIZE: {resize_window();} break;
        
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            bool isDown = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN || message == WM_LBUTTONDOWN);
            KeyCode key = keyCodes[wParam];
            handle_key_event(key, isDown);
        } break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            bool isDown = (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN \
                || message == WM_MBUTTONDOWN);
            int mouseCode = 0;
            if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP) {mouseCode = VK_LBUTTON;}
            else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP) {mouseCode = VK_RBUTTON;}
            else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP) {mouseCode = VK_MBUTTON;}
            KeyCode key = keyCodes[mouseCode];
            handle_key_event(key, isDown);
        } break;

        default: {result = DefWindowProc(hwnd,message,wParam,lParam);}
    }
    return result;
}

void platform_create_window() {
    int width = input.screenSize.x;
    int height = input.screenSize.y;
    HINSTANCE hInstance = GetModuleHandleA(0);
    WNDCLASSA wc = {};

    LPCSTR title = (LPCSTR)TEXT("NEVERWORLD");

    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    wc.lpszClassName = title;
    wc.lpfnWndProc = &window_callback;
    wc.hInstance = hInstance;
    
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

static void platform_update_window() {
    reset_input();
    MSG message;
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message); // Calls the window_callback
    }

    // Mouse Position
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(window, &mousePos);

        input.prevMousePos = input.mousePos;
        input.mousePos.x = (float)mousePos.x;
        input.mousePos.y = (float)mousePos.y;
        input.relativeMousePos = input.mousePos - input.prevMousePos;

        // Mouse Position World
        input.screenMousePos = screen_to_world(input.mousePos);
        input.prevScreenMousePos = screen_to_world(input.prevMousePos);
        input.relativeScreenMousePos = input.screenMousePos - input.prevScreenMousePos;
    }
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

void platform_fill_keycodes() {
    keyCodes[VK_LBUTTON] = KEY_MOUSE_LEFT;
    keyCodes[VK_MBUTTON] = KEY_MOUSE_MIDDLE;
    keyCodes[VK_RBUTTON] = KEY_MOUSE_RIGHT;

    keyCodes['Q'] = KEY_Q;
    keyCodes['W'] = KEY_W;
    keyCodes['E'] = KEY_E;
    keyCodes['A'] = KEY_A;
    keyCodes['S'] = KEY_S;
    keyCodes['D'] = KEY_D;
    keyCodes['Z'] = KEY_Z;
    keyCodes['X'] = KEY_X;
    keyCodes['C'] = KEY_C;

    keyCodes['1'] = KEY_1;
    keyCodes['2'] = KEY_2;
    keyCodes['3'] = KEY_3;
    keyCodes['4'] = KEY_4;
    keyCodes['5'] = KEY_5;
    
    keyCodes[VK_F11] = KEY_F11;

    keyCodes[VK_UP] = KEY_UP;
    keyCodes[VK_DOWN] = KEY_DOWN;
    keyCodes[VK_LEFT] = KEY_LEFT;
    keyCodes[VK_RIGHT] = KEY_RIGHT;

    keyCodes[VK_LSHIFT] = KEY_LSHIFT;
    keyCodes[VK_RSHIFT] = KEY_RSHIFT;

    keyCodes[VK_SPACE] = KEY_SPACE;
    keyCodes[VK_RETURN] = KEY_ENTER;
    keyCodes[VK_ESCAPE] = KEY_ESCAPE;
    keyCodes[VK_TAB] = KEY_TAB;
}

/*
KEY MAP:
    Esc                  F11
         1 2 3 4 5
    Tab  Q W E
          A S D        Enter
    Shift Z X C        Shift    /\
             Space            < \/ >
    */
