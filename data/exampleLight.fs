#version 450 core
out vec4 FragColor;

uniform mat4 u_model;
uniform mat4 u_view_projection;
uniform mat4 u_lightSpaceMatrix;

uniform sampler2D texture_diffuse0;

uniform sampler2D u_shadowMap;

uniform vec3 u_camera_pos;

uniform int u_type;

uniform vec3 u_lightPos;
uniform vec3 u_light_dir;
uniform float u_shininess;
uniform float u_diffuse_strength;
uniform vec3 u_diffuse_color;
uniform float u_spec_strength;
uniform vec3 u_spec_color;
uniform float u_cutoff;
uniform float u_outercutoff;
uniform float u_ambient_strength;
uniform vec3 u_ambient_color;


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

float ShadowCalculation(vec4 FragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range FragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}  
/*
void main() {
    vec3 color = texture(texture_diffuse0, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.0 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}
*/


/*TODO ARANAU*/
vec3 DirectionalLight()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(u_light_dir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.001);
    // specular shading
    vec3 viewDir = normalize(u_camera_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.00), u_shininess);
    // combine results
    vec3 diffuse  = u_diffuse_color  * diff * u_diffuse_strength;
    vec3 specular = u_spec_color * u_spec_strength * spec ;
    //return  diffuse + specular;
    return  diffuse ;
}  

    vec3 SpotLight(){
      vec3 color = vec3(0.0,0.0,0.0);
      vec3 n = normalize(Normal);
      vec3 viewDir = normalize(u_camera_pos - FragPos);
    //Cosas raras
      float theta = dot(u_light_dir, normalize(u_lightPos - FragPos));
      float CutOff = u_cutoff;
      float outerCutOff = u_outercutoff;
      float epsilon = CutOff - outerCutOff;
      float intensity = clamp((theta - outerCutOff)/ epsilon,0.0,1.0);

    //Difuse and specular
      float diff = max(0.0,dot(n,u_light_dir));
      vec3 reflectDir = reflect(-u_light_dir, n);  
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
      vec3 diffuse = u_diffuse_strength * diff * u_diffuse_color;
      vec3 specular = u_spec_strength * spec * u_spec_color;
      color = diffuse * intensity ;

      return color;
    }

    vec3 PointLight(){
      //General
      vec3 color = vec3(0.0,0.0,0.0);
      vec3 viewDir = normalize(u_camera_pos - FragPos);
      float distance = length(u_lightPos - FragPos);
      float constant = 1.0;
      float linear = 0.0014;
      float quadratic = 0.0007;
      float attenuation = 1.0 / (constant + linear * distance + 
                                quadratic * ( distance * distance));
      vec3 light_dir = normalize(u_lightPos - FragPos);
      vec3 n = normalize(Normal);


      //Diffuse
      float diffuse = max(dot(n,light_dir),0.0);
      color = u_diffuse_color * diffuse * attenuation * u_diffuse_strength ;

      //Specular
      vec3 view_dir = normalize(u_camera_pos - FragPos);
      vec3 reflect_dir = reflect(-light_dir,n);
      float specular = pow(max(dot(view_dir,reflect_dir),0.0),u_shininess);
      vec3 spe_color = u_spec_color * specular * attenuation * u_spec_strength;
      return (color );
    }

    vec3 AmbientLight(){
            return  u_ambient_strength * u_ambient_color;      
    }


void main() {


//create light
//funciona type 1
  vec3 light = vec3(0.0,0.0,0.0);


        switch(u_type){
          case 1:
            light = DirectionalLight();
            float shadow = ShadowCalculation(FragPosLightSpace);
            light = light * (1.0 - shadow);
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