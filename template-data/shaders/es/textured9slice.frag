#version 300 es
precision mediump float;

uniform vec4 color;
uniform vec2 dimensions;
uniform vec2 border;
uniform vec2 textureSize;
uniform sampler2D inputTexture;

in vec2 texCoord;
out vec4 fragColor;

float map(float value, float originalMin, float originalMax, float newMin, float newMax) {
    return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
}

float processAxis(float coord, float textureBorder, float windowBorder) {
    if (coord < windowBorder)
        return map(coord, 0.0, windowBorder, 0.0, textureBorder) ;
    if (coord < 1.0 - windowBorder) 
        return map(coord,  windowBorder, 1.0 - windowBorder, textureBorder, 1.0 - textureBorder);
    return map(coord, 1.0 - windowBorder, 1.0, 1.0 - textureBorder, 1.0);
}

void main() {
    vec2 newUV = vec2(
        processAxis(texCoord.x, border.x, dimensions.x),
        processAxis(texCoord.y, border.y, dimensions.y)
    );
    fragColor = texture(inputTexture, newUV);
    // fragColor = texture(inputTexture, texCoord.xy) * color;
}