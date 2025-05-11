#pragma once
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>

// ######################## DEFINES #############################

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
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

inline int clamp(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}

#define BIT(x) (1 << (x))
#define KB(x) ((x) * (unsigned long long)1024)
#define MB(x) (KB(x) * (unsigned long long)1024)
#define GB(x) (MB(x) * (unsigned long long)1024)

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

// ######################## BUMP ALLOCATOR #############################

struct BumpAllocator {
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size) {
    BumpAllocator ba = {};
    ba.memory = (char*)malloc(size);
    SM_ASSERT(ba.memory,"Failed to allocate memory!");
    ba.capacity = size;
    memset(ba.memory, 0, size); // Sets the memory to 0
    return ba;
}

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size) {
    char* result = nullptr;
    size_t allignedSize = (size + 7) & ~ 7; // first 4 bits is 0
    SM_ASSERT(bumpAllocator->used + allignedSize > bumpAllocator->capacity, 
        "BumpAllocator is full!");
    result = bumpAllocator->memory + bumpAllocator->used;
    bumpAllocator->used += allignedSize;
    printf("Allocated %d bytes of memory\n",(unsigned long long)allignedSize);
    return result;
}

// ######################## File I/O #############################

/**
 * Get the time in milliseconds from when the file was loaded.
 */
long long get_timestamp(char* file) {
    struct stat file_stat = {};
    stat(file, &file_stat);
    return file_stat.st_mtime;
}

bool file_exists(char* filePath) {
    SM_ASSERT(filePath, "No filePath supplied!");

    auto file = fopen(filePath, "rb");
    if (!file) return false;
    fclose(file);
    return true;
}

long get_file_size(char* filePath) {
    SM_ASSERT(filePath,"No filePath supplied!");

    long fileSize = 0;
    auto file = fopen(filePath, "rb"); // read in binary
    if (!file) {
        SM_ERROR("Failed opening File: %s", filePath);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return fileSize;
}

/**
 * Reads a file into a supplied buffer. 
 * We manage our own memory and want more control over where it is allocated.
 */
char* read_file(char* filePath, int* fileSize, char* buffer) {
    SM_ASSERT(filePath, "No filePath supplied!");
    SM_ASSERT(fileSize, "No fileSize supplied!");
    SM_ASSERT(buffer, "No buffer supplied!");

    *fileSize = 0;
    auto file = fopen(filePath,"rb");
    if (!file) {
        SM_ERROR("Failed opening File: %s", filePath);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer, 0, *fileSize + 1); // Null terminated just in case

    fread(buffer, sizeof(char), *fileSize, file);
    fclose(file);
    return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpAllocator) {
    char* file = nullptr;
    long fileSize2 = get_file_size(filePath);
    if(fileSize2) {
        char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);
        file = read_file(filePath, fileSize, buffer);
    }
    return file;
}

void write_file(char* filePath, char* buffer, int size) {
    SM_ASSERT(filePath, "No filePath supplied!");
    SM_ASSERT(buffer, "No buffer supplied!");

    auto file = fopen(filePath,"wb");
    if (!file) {
        SM_ERROR("Failed opening File: %s", filePath);
        return;
    }

    fwrite(buffer, sizeof(char), size, file);
    fclose(file);
}

bool copy_file(char* fileName, char* outputName, char* buffer) {
    int fileSize = 0;
    char* data = read_file(fileName, &fileSize, buffer);
    auto outputFile = fopen(outputName,"wb");
    if (!outputFile) {
        SM_ERROR("Failed opening File: %s", outputName);
        return false;
    }

    int result = fwrite(data, sizeof(char), fileSize, outputFile);
    if (!result) {
        SM_ERROR("Failed opening File: %s", outputName);
        return false;
    }

    fclose(outputFile);
    return true;
}

bool copy_file(char* fileName, char* outputName, BumpAllocator* bumpAllocator) {
    char* file = 0;
    long fileSize2 = get_file_size(fileName);

    if(fileSize2) {
        char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);
        return copy_file(fileName,outputName,buffer);
    }
    return false;
}

// ######################## Math Structs #############################

struct Vec2 {
    float x;
    float y;
    Vec2 operator-(Vec2 v) {return {x - v.x, y - v.y};}
    Vec2 operator+(Vec2 v) {return {x + v.x, y + v.y};}
};

struct Vec2i {
    int x;
    int y;
    Vec2i operator-(Vec2i v) {return {x - v.x, y - v.y};}
    Vec2i operator+(Vec2i v) {return {x + v.x, y + v.y};}
    Vec2i operator/(int s) {return {x / s, y / s};}
};

struct Vec4 {
    union {
        float values[4];
        struct {float x, y, z, w;};
        struct {float r, g, b, a;};
    };
    float& operator[](int index) {
        return values[index];
    }
};
struct Mat4 {
    union {
        Vec4 values[4]; //Column or Row major?
        struct {
            float ax,bx,cx,dx;
            float ay,by,cy,dy;
            float az,bz,cz,dz;
            float aw,bw,cw,dw;
        };
    };

    Vec4& operator[](int column) {
        return values[column];
    }
};

Mat4 orthographic_projection(float left, float right, float top, float bottom) {
    Mat4 result = {};
    result.aw = -(right + left) / (right - left);
    result.bw = (top + bottom) / (top - bottom);
    result.cw = 0.0f; // Near Plane (flat)
    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = 1.0f; // Far Plane
    result[3][3] = 1.0f; // Homogeneous coordinate

    return result;
}