#version 420 core


out vec4 FragColor;

in VS_OUT
{
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

// material parameters
struct Texture2D_RGB
{
    sampler2D value;
    int set;
    vec3 default_value;
};

struct Texture2D_R
{
    sampler2D value;
    int set;
    float default_value;
};

uniform Texture2D_RGB albedo_tex;
uniform Texture2D_RGB normal_tex;
uniform Texture2D_R metallic_tex;
uniform Texture2D_R roughness_tex;
uniform Texture2D_R ao_tex;

// lights
struct LightSource
{
    vec3 position;
    int type;
    vec3 direction;
    float constant;
    vec3 color;
    float linear;
    float quadratic;
    float cutoff;
    float outer_cutoff;
};
#define LIGHTS_COUNT_MAX 20
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2
layout (std140, binding = 1) uniform Lights
{
    LightSource lights_sources[LIGHTS_COUNT_MAX];
};
uniform int lights_count;

uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_tex.value, vs_out.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vs_out.WorldPos);
    vec3 Q2  = dFdy(vs_out.WorldPos);
    vec2 st1 = dFdx(vs_out.TexCoords);
    vec2 st2 = dFdy(vs_out.TexCoords);

    vec3 N   = normalize(vs_out.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 albedo = albedo_tex.set * pow(texture(albedo_tex.value, vs_out.TexCoords).rgb, vec3(2.2)) - (albedo_tex.set - 1) * albedo_tex.default_value;
    float metallic = metallic_tex.set * texture(metallic_tex.value, vs_out.TexCoords).r - (metallic_tex.set - 1) * metallic_tex.default_value;
    float roughness = roughness_tex.set * texture(roughness_tex.value, vs_out.TexCoords).r - (roughness_tex.set - 1) * roughness_tex.default_value;
    float ao = ao_tex.set * texture(ao_tex.value, vs_out.TexCoords).r - (ao_tex.set - 1) * ao_tex.default_value;

    vec3 N = normalize(vs_out.Normal);
    if (normal_tex.set > 0)
    {
        N = getNormalFromMap();
    }
    vec3 V = normalize(camPos - vs_out.WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lights_count; ++i) 
    {
        vec3 L = normalize(lights_sources[i].position - vs_out.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lights_sources[i].position - vs_out.WorldPos);
        
        float attenuation = 1.0;
        float intensity = 1.0;

        // attenuation
        if (lights_sources[i].type >= LIGHT_POINT)
        {
            attenuation = 1.0 / (lights_sources[i].constant + lights_sources[i].linear * distance + lights_sources[i].quadratic * (distance * distance));    
        }
        // spot light intensity
        if (lights_sources[i].type == LIGHT_SPOT)
        {
            float theta = dot(L, normalize(-lights_sources[i].direction)); 
            float epsilon = lights_sources[i].cutoff - lights_sources[i].outer_cutoff;
            intensity = clamp((theta - lights_sources[i].outer_cutoff) / epsilon, 0.0, 1.0);
        }

        // radiance
        vec3 radiance = lights_sources[i].color * attenuation * intensity;

        // calculate per-light radiance
        // vec3 L = normalize(lightPositions[i] - vs_out.WorldPos);
        // vec3 H = normalize(V + L);
        // float distance = length(lightPositions[i] - vs_out.WorldPos);
        // float attenuation = 1.0 / (distance * distance);
        // vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}

