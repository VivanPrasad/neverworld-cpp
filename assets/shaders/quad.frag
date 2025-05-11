#version 430 core

// Input
layout (location = 0) in vec2 textureCoordsIn; // Interpolated texture coordinates

// Output
layout (location = 0) out vec4 fragColor;

// Bindings
layout (location = 0) uniform sampler2D textureAtlas;

void main() {
    // Set the output color to white
    vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoordsIn), 0); // Convert into integer coords
    if (textureColor.a == 0.0) {discard;} // Discard the fragment if the alpha is too low
    
    fragColor = textureColor;
}