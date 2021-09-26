#version 330 core
precision mediump float;

uniform vec4 color = vec4(1, 1, 1, 1);

out vec4 fragColor;

void main()
{
    fragColor = color;
}