#version 330 core
layout (location = 0) in vec3 aPos;        // Vertex position
layout (location = 1) in vec3 aNormal;     // Vertex normal
layout (location = 2) in vec2 aTexCoords;  // Texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D displacementMap;
uniform float time;

out vec3 Normal;    
out vec2 uv;        

void main() {
    vec3 displaced = aPos;
    float displacement = texture(displacementMap, aTexCoords + vec2(time * 0.05, 0)).r;  
    displaced.y += displacement * 0.2;  

    gl_Position = projection * view * model * vec4(displaced, 1.0);
    
    Normal = aNormal;
    uv = aTexCoords;
}
