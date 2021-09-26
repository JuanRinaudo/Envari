#version 300 es
precision mediump float;

uniform vec4 color;
uniform vec2 dimensions;
uniform vec2 border;
uniform vec2 textureSize;
uniform sampler2D inputTexture;

in vec2 texCoord;
out vec4 fragColor;

float mapFunction(float value, float originalMin, float originalMax, float newMin, float newMax) {
    return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
}

float processAxis(float coord, float textureBorder, float windowBorder) {
    if (coord < windowBorder)
        return mapFunction(coord, 0.0f, windowBorder, 0.0f, textureBorder) ;
    if (coord < 1.0f - windowBorder)
        return mapFunction(coord,  windowBorder, 1.0f - windowBorder, textureBorder, 1.0f - textureBorder);
    return mapFunction(coord, 1.0f - windowBorder, 1.0f, 1.0f - textureBorder, 1.0f);
}

void main() {
    vec2 newUV = vec2(
        processAxis(texCoord.x, border.x, dimensions.x),
        processAxis(texCoord.y, border.y, dimensions.y)
    );
    fragColor = texture(inputTexture, newUV) * color;
}