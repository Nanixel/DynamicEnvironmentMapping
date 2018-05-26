#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPosition;
//in vec2 TexCoord;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(FragPosition - cameraPos);
    // r = 2 * Dot(N, I) * N
    vec3 R = reflect(I, Normal);
    FragColor = texture(skybox, R);
}
