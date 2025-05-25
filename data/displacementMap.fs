#version 330 core

out vec4 FragColor;

in vec2 uv;
in vec3 Normal;  

uniform sampler2D waterDiffuse;  
uniform sampler2D waterNormal;   
void main()
{
    vec3 diffuseColor = texture(waterDiffuse, uv).rgb;

    vec3 normalMap = texture(waterNormal, uv).rgb;
    normalMap = normalize(normalMap * 2.0 - 1.0);  

    vec3 finalColor = diffuseColor * 0.8; /

    FragColor = vec4(finalColor, 1.0f);
}
