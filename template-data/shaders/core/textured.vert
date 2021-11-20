#version 330 core
precision mediump float;

uniform mat4 mvp;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
out vec2 texCoord;

void main()
{
   texCoord = uv;
   gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);
}