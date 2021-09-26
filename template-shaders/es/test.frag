#version 300 es
precision mediump float;

uniform float time;
uniform vec2 speed;
uniform vec4 color;
uniform sampler2D textureSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture(textureSampler, mod(texCoord.xy + time * speed, 1.0f)) * color;
}