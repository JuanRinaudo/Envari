#version 330 core
precision mediump float;

flat in int instanceID;
in vec4 colorFrag;
out vec4 fragColor;

void main()
{
    fragColor = colorFrag;
}