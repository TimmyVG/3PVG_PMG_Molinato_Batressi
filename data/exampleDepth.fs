#version 330 core
uniform vec3 u_lightPos;
uniform float u_far;
in vec4 FragPos;


uniform int u_type;
void main()
{             
     

     // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - u_lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / u_far;
    
    // write this as modified depth
    if(u_type == 2){
     gl_FragDepth = lightDistance;
     }else{    
          gl_FragDepth = gl_FragCoord.z;

    }
}  


