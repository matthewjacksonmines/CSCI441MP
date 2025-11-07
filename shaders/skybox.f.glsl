#version 410 core

in vec3 vDir;
out vec4 fragColor;
uniform samplerCube uCube;

void main() {
    fragColor = texture(uCube, normalize(vDir));
}