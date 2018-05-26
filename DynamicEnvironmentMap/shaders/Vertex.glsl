#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 coord;
layout (location = 2) in vec3 normal;

//out vec2 TexCoord;

out vec3 Normal;
out vec3 FragPosition;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    //TexCoord = coord;
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPosition = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
