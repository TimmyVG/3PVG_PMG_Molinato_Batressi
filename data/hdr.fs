#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;
uniform int toneMapType; //0 = NONE,  1 = Reinhard, 2 = Exposure, 3 = ACES, 4 = Uncharted2
uniform float gamma;
vec3 mapped;
vec3 Tonemap_Reinhard(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 Tonemap_Exposure(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}

vec3 Tonemap_ACES(vec3 color) {
    // ACES approximation by Narkowicz
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

vec3 Tonemap_Uncharted2(vec3 color) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    return color / white;
}

void main()
{             
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr)
    {
        // reinhard
        if (toneMapType == 0)
          mapped =  hdrColor / (hdrColor + vec3(1.0));
        else if (toneMapType == 1)
            mapped = Tonemap_Reinhard(hdrColor);
        else if (toneMapType == 2)
            mapped = Tonemap_Exposure(hdrColor, exposure);
        else if (toneMapType == 3)
            mapped = Tonemap_ACES(hdrColor);
        else if (toneMapType == 4)
            mapped = Tonemap_Uncharted2(hdrColor);
        else
            mapped = hdrColor; // sin tonemapping
        // exposure
        //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        mapped = pow(mapped, vec3(1.0 / gamma));
        FragColor = vec4(mapped, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}