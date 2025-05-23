#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 u_model;
uniform mat4 u_view_projection;

uniform mat4 u_view;
void main()
{
    vec4 worldPos = u_model * vec4(aPos, 1.0);
    vec4 viewPos = u_view * worldPos;
    FragPos = viewPos.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    vec3 normal_world = normalize(normalMatrix * aNormal);
    Normal = normalize(mat3(u_view) * normal_world);

    gl_Position = u_view_projection * worldPos;
}