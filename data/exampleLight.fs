#version 450 core
out vec4 FragColor;

uniform mat4 u_model;
uniform mat4 u_view_projection;
uniform mat4 u_lightSpaceMatrix;

uniform sampler2D texture_diffuse0;

uniform sampler2D u_shadowMap;

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



in vec3 FragPos;
in vec3 normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;



vec3 DirectionalLight()
{
  vec3 viewDir = normalize(u_camera_pos - FragPos);
  vec3 lightDir = normalize(u_light_dir);
  vec3 Normal = normalize(normal);
  float diff = max(dot(Normal,lightDir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-lightDir,Normal);
  float spec = pow(max(dot(viewDir,reflectDir),0.0), u_shininess);

  // Combine results

  return (u_diffuse_color * u_diffuse_strength * diff + u_spec_color * u_spec_strength * spec);

}  

    vec3 SpotLight(){
      vec3 viewDir = normalize(u_camera_pos - FragPos);
      vec3 lightDir = normalize(u_lightPos - FragPos);
      vec3 Normal = normalize(normal);
      float diff = max(dot(Normal, lightDir),0.0f);

      vec3 reflectDir = reflect(-lightDir,Normal);
      float spec = pow(max(dot(viewDir,reflectDir),0.0),u_shininess);
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
      float diff = max(dot(Normal, lightDir),0.0);

      vec3 reflectDir = reflect(-lightDir,Normal);
      float spec = pow(max(dot(viewDir, reflectDir),0.0), u_shininess);

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


        switch(u_type){
          case 1:
            light = DirectionalLight();
            //float shadow = ShadowCalculation(FragPosLightSpace);
            //light = light * (1.0 - shadow);
        break;

        case 2:
            light = PointLight();
        break;

        case 3:
            light = SpotLight();
            break;
        case 4:
            light = AmbientLight();
            break;
        }
      
     
    FragColor = texture(texture_diffuse0,TexCoords) * vec4(light ,1.0) ; 
    


}