#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;



uniform mat4 u_model;
uniform mat4 u_view_projection;
uniform mat4 u_lightSpaceMatrix;


out vec3 FragPos;
out vec3 normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;


void main() {
    gl_Position = u_view_projection * u_model * vec4(aPos,1.0);
    TexCoords = aTexCoords;
    normal = mat3(transpose(inverse(u_model))) * aNormal;
    FragPos = vec3(u_model * vec4(aPos, 1.0));
    FragPosLightSpace = u_lightSpaceMatrix * vec4(FragPos, 1.0);
}
