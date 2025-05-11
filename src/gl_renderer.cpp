#include "gl_renderer.h"

// To Load PNG files
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ###################### OPENGL CONSTANTS ##################################
const char* TEXTURE_PATH = "assets/textures/TEXTURE_ATLAS.png";

// ###################### OPENGL STRUCTS ##################################
struct GLContext {
    GLuint programID;
    GLuint textureID;
};

// ###################### OPENGL GLOBALS ##################################
static GLContext glContext;

// ###################### OPENGL FUNCTIONS ##################################
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar* message, const void* user) {
    // Handle OpenGL debug messages
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW:
            SM_ASSERT(false,"OpenGL Error: %s", message);
            break;
        default:
            SM_TRACE((char*)message);
            break;
    }
}

bool gl_init(BumpAllocator* transientStorage) {
    // Block of memory for memory allocation
    load_gl_functions();
    glDebugMessageCallback((GLDEBUGPROC)&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);
    if (!vertShader || !fragShader) {
        SM_ASSERT(false, "Failed to load shader files");
        return false;
    }

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);

    // Test if vertex shaders compiled successfully
    {
        int success;
        char shaderLog[2048] = {};
        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Vertex Shader Compilation Error: %s", shaderLog);
        }
    }

    // Test if fragment shaders compiled successfully
    {
        int success;
        char shaderLog[2048] = {};
        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Frag Shader Compilation Error: %s", shaderLog);
        }
    }

    glContext.programID = glCreateProgram();
    glAttachShader(glContext.programID, vertShaderID);
    glAttachShader(glContext.programID, fragShaderID);
    glLinkProgram(glContext.programID);
    
    glDetachShader(glContext.programID, vertShaderID);
    glDetachShader(glContext.programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    // Unused but neeeded for some reason
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Texture loading
    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);
        SM_ASSERT(data, "Failed to load texture: %s", TEXTURE_PATH);
        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // TexelFetch Coordinates, ignoring filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 
            0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    // Same color space as the texture
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_BLEND); // Disable multisampling

    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);
    
    return true;
}

void gl_render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input.screenWidth, input.screenHeight);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}