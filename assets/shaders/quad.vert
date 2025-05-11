#version 430 core

// Input

// Output
layout (location = 0) out vec2 textureCoordsOut;

void main() {
    // Generating Vertices on the GPU
    // mostly because we have a 2D Engine

    // OpenGL Coordinates
    // -1, 1             1, 1
    // -1,-1             1,-1

    vec2 vertices[6] =  {
        vec2(-0.5, 0.5), // Top Left
        vec2(-0.5, -0.5), // Bottom Left
        vec2(0.5, 0.5), // Top Right
        vec2(0.5, 0.5), // Top Right
        vec2(-0.5, -0.5), // Bottom Left
        vec2(0.5, -0.5), // Bottom Right
    };

    float left = 0.0;
    float top = 0.0;
    float right = 8.0;
    float bottom = 16.0;

    vec2 textureCoords[6] = {
        vec2(left, top), // Top Left
        vec2(left, bottom), // Bottom Left
        vec2(right, top), // Top Right
        vec2(right, top), // Top Right
        vec2(left, bottom), // Bottom Left
        vec2(right, bottom) // Bottom Right
    };
    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    textureCoordsOut = textureCoords[gl_VertexID];
}