#pragma once
#include <stdio.h>

#ifdef _WIN32
    #ifdef _MSC_VER
        #define DEBUG_BREAK() __debugbreak() // MSVC
    #else
        #define DEBUG_BREAK() __builtin_trap() // GCC/Clang
    #endif
#else
    #define DEBUG_BREAK() __builtin_trap() // Non-Windows platforms
#endif
/**
 * Official header file for the NeverLib library.
 */

// ######################## DEFINES #############################
enum TextColor {
    TEXT_COLOR_DEFAULT,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_GREY,
    TEXT_COLOR_COUNT
};
// ######################## LOGGING #############################

template <typename... Args>
void _log(const char* prefix, const char* msg, TextColor color, Args... args) {
    const char* COLORS[TEXT_COLOR_COUNT] = {
        "\x1b[0m", // Default
        "\x1b[31m", // Red
        "\x1b[32m", // Green
        "\x1b[33m", // Yellow
        "\x1b[34m", // Blue
        "\x1b[35m", // Magenta
        "\x1b[36m", // Cyan
        "\x1b[37m", // White
        "\x1b[31m", // Grey
    };
    char formatBuffer[8192] = {};
    sprintf(formatBuffer,"%s %s %s \033[0m", COLORS[color],prefix,msg);
    char textBuffer[8912] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...)  _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...) \
{\
    if (!x)\
    {\
        SM_ERROR(msg, ##__VA_ARGS__);\
        SM_ERROR("Assertion Hit");\
        DEBUG_BREAK();\
    }\
}
