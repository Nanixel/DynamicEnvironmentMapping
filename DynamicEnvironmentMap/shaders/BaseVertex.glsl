#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 coord;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoords;

void main() {
    TexCoords = coord;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}

