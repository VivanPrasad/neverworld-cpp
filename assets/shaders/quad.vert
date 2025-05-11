#version 430 core

// Structs
struct Transform {
    vec2 pos;
    vec2 size;
    ivec2 atlasOffset;
    ivec2 spriteSize;
};
// Input
layout (std430, binding = 0) buffer TransformSBO {
    Transform transforms[]; // Hold array of transforms handled by CPU
};

uniform vec2 screenSize; // Screen size for normalizing coordinates
uniform mat4 projection; // Projection matrix for 3D orthogrpahic rendering

// Output
layout (location = 0) out vec2 textureCoordsOut;

void main() {

    Transform transform = transforms[gl_InstanceID];

    // Generating Vertices on the GPU
    // mostly because we have a 2D Engine

    // OpenGL Coordinates
    // -1, 1             1, 1
    // -1,-1             1,-1

    vec2 vertices[6] =  {
        transform.pos, // Top Left
        vec2(transform.pos + vec2(0.0, transform.size.y)), // Bottom Left
        vec2(transform.pos + vec2(transform.size.x, 0.0)), // Top Right
        vec2(transform.pos + vec2(transform.size.x, 0.0)), // Top Right
        vec2(transform.pos + vec2(0.0, transform.size.y)), // Bottom Left
        transform.pos + transform.size // Bottom Right
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bottom = transform.atlasOffset.y + transform.spriteSize.y;

    vec2 textureCoords[6] = {
        vec2(left, top), // Top Left
        vec2(left, bottom), // Bottom Left
        vec2(right, top), // Top Right
        vec2(right, top), // Top Right
        vec2(left, bottom), // Bottom Left
        vec2(right, bottom) // Bottom Right
    };
    
    // Normalized Position
    {
        vec2 vertexPos = vertices[gl_VertexID];
        //vertexPos.y = -vertexPos.y + screenSize.y; // Invert Y axis
        //vertexPos = 2.0 * (vertexPos / screenSize) - 1.0; // Normalize X axis
        gl_Position = projection * vec4(vertexPos, 0.0, 1.0); // Set position
    }

    textureCoordsOut = textureCoords[gl_VertexID];
}