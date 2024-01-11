#version 410 core
layout (location = 0) in vec3 inPos;

out vec4 vertexColor;

void main() {
    gl_Position = vec4(inPos, 1.0);
    vertexColor = vec4(inPos.xyz, 1.0);
}