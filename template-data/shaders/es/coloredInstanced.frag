#version 300 es
precision mediump float;

flat in int instanceID;
in vec4 colorFrag;
out vec4 fragColor;

uniform int count;

void main()
{
    fragColor = colorFrag;
}