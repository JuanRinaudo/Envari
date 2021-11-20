#version 300 es
precision mediump float;

uniform vec4 color;
uniform sampler2D inputTexture;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    fragColor = vec4(color.rgb, texture(inputTexture, texCoord.xy));
}