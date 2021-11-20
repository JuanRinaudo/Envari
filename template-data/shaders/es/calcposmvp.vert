#version 300 es

layout (location = 0) in vec2 position;

out mat4 mvp;

uniform vec2 scale;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    mat4 mPos = mat4(1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                position.x, position.y, 0.0, 1.0);
                
    mat4 mScale = mat4(scale.x, 0.0, 0.0, 0.0,
                0.0, scale.y, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0);

    mat4 model = mPos * mScale;
    mvp = projection * view * model;
}