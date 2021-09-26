#version 330 core
precision mediump float;

uniform mat4 mvp;

layout (location = 0) in vec3 position;

void main()
{
   gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);
}