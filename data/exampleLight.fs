#version 450 core
out vec4 FragColor;

uniform mat4 u_model;
uniform mat4 u_view_projection;
uniform mat4 u_lightSpaceMatrix;

uniform sampler2D texture_diffuse0;

uniform sampler2D u_shadowMap;
uniform samplerCube u_cubeMap;

uniform vec3 u_camera_pos;
//TODO
uniform vec3 u_camera_dir;

uniform int u_type;

uniform vec3 u_lightPos;
uniform vec3 u_light_dir;

//PointLight data
//TODO
uniform float u_constant;
uniform float u_linear;
uniform float u_quadratic;

//SpotLight
uniform float u_cutoff;
uniform float u_outercutoff;

uniform float u_shininess;

uniform float u_diffuse_strength;
uniform vec3 u_diffuse_color;

uniform float u_spec_strength;
uniform vec3 u_spec_color;

uniform int u_blin;

uniform float u_near;
uniform float u_far;

in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

float ShadowCalculationDefault(float bias)
{
    vec3 Normal = normalize(normal);
    
    //float bias = max(0.05 * (1.0 - dot(Normal, normalize(u_light_dir))), 0.05);  
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // PCF sampling
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);

    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2(-1, -1) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 0, -1) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 1, -1) * texelSize).r ? 1.0 : 0.0;

    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2(-1,  0) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 0,  0) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 1,  0) * texelSize).r ? 1.0 : 0.0;

    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2(-1,  1) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 0,  1) * texelSize).r ? 1.0 : 0.0;
    shadow += currentDepth - bias > texture(u_shadowMap, projCoords.xy + vec2( 1,  1) * texelSize).r ? 1.0 : 0.0;

    shadow /= 9.0;

    return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);   
float ShadowCalculationPoint(float bias)
{
    vec3 fragToLight = FragPos - u_lightPos;

    float currentDepth = length(fragToLight);
    // 20 points
    float shadow = 0.0;
    int samples = 20;
    float closestDepth;
    float viewDistance = length(u_camera_pos - FragPos);
    float diskRadius = (1.0 + (viewDistance / u_far)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        closestDepth = texture(u_cubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= u_far;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
    return shadow;
}

vec3 DirectionalLight()
{
  vec3 viewDir = normalize(u_camera_pos - FragPos);
  vec3 lightDir = normalize(-u_light_dir);
  vec3 Normal = normalize(normal);
  float diff = max(dot(Normal,lightDir), 0.0);

  float spec = 0.0;
  if(u_blin == 1){
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(Normal, halfwayDir),0.0),u_shininess * 2.0);
  }else{
    vec3 reflectDir = reflect(-lightDir,Normal);
    spec = pow(max(dot(viewDir,reflectDir),0.0),u_shininess);
  }

  // Combine results

  return (u_diffuse_color * u_diffuse_strength * diff + u_spec_color * u_spec_strength * spec);

}  

    vec3 SpotLight(){
      vec3 viewDir = normalize(u_camera_pos - FragPos);
      vec3 lightDir = normalize(u_lightPos - FragPos);
      vec3 Normal = normalize(normal);
      float diff = max(dot(Normal, lightDir),0.0f);

      float spec = 0.0;
      if(u_blin == 1){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(Normal, halfwayDir),0.0),u_shininess * 2.0);
      }else{
        vec3 reflectDir = reflect(-lightDir,Normal);
        spec = pow(max(dot(viewDir,reflectDir),0.0),u_shininess);
      }
      // Attenuation
      float distance = length(u_lightPos - FragPos);
      float attenuation = 1.0 / (u_constant + u_linear * distance + u_quadratic * (distance * distance));

      // Spotlight intensity
      float theta = dot(lightDir, normalize(-u_light_dir));
      float epsilon = u_cutoff - u_outercutoff;
      float intensity = clamp((theta - u_outercutoff) / epsilon, 0.0, 1.0);

      //Combine
      vec3 diffuse = u_diffuse_color * u_diffuse_strength * diff;
      vec3 specular = u_spec_color * u_spec_strength * spec;

      diffuse *= attenuation * intensity;
      specular *= attenuation * intensity; 
      return diffuse + specular;

    }

    vec3 PointLight(){
      //General
      vec3 viewDir = normalize(u_camera_pos - FragPos);
      vec3 lightDir = normalize(u_lightPos - FragPos);
      vec3 Normal = normalize(normal);
      float diff = max(dot(lightDir, Normal),0.0);
      float spec = 0.0;
      
      if(u_blin == 1){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(Normal, halfwayDir),0.0),u_shininess * 2.0);
      }else{
        vec3 reflectDir = reflect(-lightDir,Normal);
        spec = pow(max(dot(viewDir,reflectDir),0.0),u_shininess);
      }

      float distance = length(u_lightPos - FragPos);
      float attenuation = 1.0 / (u_constant + u_linear * distance + u_quadratic * (distance * distance));
  
      vec3 diffuse = u_diffuse_color * u_diffuse_strength * diff;
      vec3 specular = u_spec_color * u_spec_strength * spec;

      diffuse *= attenuation;
      specular *= attenuation;
      return (diffuse + specular);
    }

    vec3 AmbientLight(){
            return  u_spec_color * u_spec_strength;      
    }


void main() {


//create light
//funciona type 1

  vec3 light = vec3(0.0,0.0,0.0);
    float shadow = 0.0f;
    float bias = 0.0005;
        switch(u_type){
          case 1:
            light = DirectionalLight();
            shadow = ShadowCalculationDefault(max(0.05 * (1.0 - dot(normalize(normal), normalize(u_light_dir))), 0.05));
            //light = light * (1.0 - shadow);
        break;

        case 2:
            light = PointLight();
            shadow = ShadowCalculationPoint(0.15);
        break;

        case 3:
            light = SpotLight();
            shadow = ShadowCalculationDefault(bias);
            break;
        case 4:
            light = AmbientLight();
            break;
        }
      


      FragColor = texture(texture_diffuse0,TexCoords) * vec4(light ,1.0) * (1.0 - shadow)  ; 



}