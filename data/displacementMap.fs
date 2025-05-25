#version 330 core

out vec4 FragColor;

in vec2 uv;
in vec3 Normal;  
in float displacement;

uniform sampler2D waterDiffuse;  
uniform sampler2D waterNormal;
uniform sampler2D displacementMap;

uniform float time;

void main()
{
    vec3 color = texture(waterDiffuse, uv).rgb;

    FragColor = vec4(color, 1.0);
}
