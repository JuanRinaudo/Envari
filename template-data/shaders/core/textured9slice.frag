#version 330 core
precision mediump float;

uniform vec4 color;
uniform vec2 dimensions;
uniform vec2 border;
uniform vec2 textureSize;
uniform sampler2D texture;

in vec2 texCoord;
out vec4 fragColor;

float map(float value, float originalMin, float originalMax, float newMin, float newMax) {
    return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
}

float processAxis(float coord, float textureBorder, float windowBorder) {
    if (coord < windowBorder)
        return map(coord, 0, windowBorder, 0, textureBorder) ;
    if (coord < 1 - windowBorder) 
        return map(coord,  windowBorder, 1 - windowBorder, textureBorder, 1 - textureBorder);
    return map(coord, 1 - windowBorder, 1, 1 - textureBorder, 1);
}

void main() {
    vec2 newUV = vec2(
        processAxis(texCoord.x, border.x, dimensions.x),
        processAxis(texCoord.y, border.y, dimensions.y)
    );
    fragColor = texture2D(texture, newUV);
    // fragColor = texture2D(texture, texCoord.xy) * color;
}