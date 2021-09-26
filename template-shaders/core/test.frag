#version 330 core
precision mediump float;

uniform float time;
uniform vec2 speed = vec2(.2, .2);
uniform vec4 color = vec4(1, 1, 1, 1);
uniform sampler2D texture;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture2D(texture, mod(texCoord.xy + time * speed, 1)) * color;
}