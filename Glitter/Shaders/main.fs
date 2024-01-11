#version 410 core
out vec4 FragColor;

in vec4 vertexColor;

uniform float time;

void main()
{
    FragColor = vec4(0.5f, sin(time), 0.5f, 1.0f);
}