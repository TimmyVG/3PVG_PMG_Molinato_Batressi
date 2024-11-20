#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

in vec2 uv;

uniform sampler2D texture_diffuse1;
uniform vec3 ourColor;
void main() {
    FragColor = texture(texture_diffuse1,uv);
}
