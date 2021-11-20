#version 300 es
precision mediump float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 mvp;

out vec2 texCoord;

void main()
{
   texCoord = uv;
   gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);
}