#version 330 core
precision mediump float;

uniform vec4 color;
uniform sampler2D texture;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    fragColor = vec4(color.rgb, texture2D(texture, texCoord.xy).a);
}