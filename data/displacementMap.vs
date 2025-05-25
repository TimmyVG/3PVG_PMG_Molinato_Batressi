#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D displacementMap;
uniform float time;

out vec3 Normal;
out vec2 uv;

void main() {
    
    vec2 animatedUV = aTexCoords + 0.05 * time;
    
    float displacement = texture(displacementMap, animatedUV).r * sin(time * 2.0) * cos(time * 1.5);

    vec3 displacedPosition = aPos + aNormal * displacement * 2;
    gl_Position = projection * view * model * vec4(displacedPosition, 1.0);

    Normal = aNormal;
    uv = animatedUV;
}
