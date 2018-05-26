#version 330 core

layout (location = 0) in vec3 position;

//Remember the texture coords of a skybox are its positions
out vec3 TextureCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = projection * view * vec4(position, 1.0);
    TextureCoords = position;
    gl_Position = pos.xyww;
}
