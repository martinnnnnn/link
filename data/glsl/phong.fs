#version 420 core

// struct DirectionalLight
// {
//     vec3 direction;
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// struct PointLight
// {
//     vec3 position;
//     float constant;
//     vec3 ambient;
//     float linear;
//     vec3 diffuse;
//     float quadratic;
//     vec3 specular;
//     bool is_on;
// };

// struct SpotLight
// {
//     vec3 position;
//     vec3 direction;
//     float cutoff;
//     float outer_cutoff;
  
//     float constant;
//     float linear;
//     float quadratic;
  
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;       
// };

// #define POINT_LIGHTS_MAX 20

// uniform DirectionalLight directional_light;

// layout (std140, binding = 1) uniform point_lights
// {
//     PointLight PointLights[POINT_LIGHTS_MAX];
// };
// uniform int lights_count;

// uniform SpotLight spot_light;

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

uniform Texture2D_RGB diffuse_tex;
uniform Texture2D_RGB normal_tex;
uniform Texture2D_RGB specular_tex;
uniform Texture2D_RGB ambient_tex;
uniform Texture2D_R shininess_tex;

// uniform sampler2D diffuse_tex;
// uniform sampler2D normal_tex;
// uniform sampler2D specular_tex;
// uniform sampler2D ambient_tex;
// uniform sampler2D shininess_tex;

// uniform vec3 diffuse_default;
// uniform vec3 specular_default;
// uniform vec3 ambient_default;
// uniform float shininess_default;

// uniform bool diffuse_set;
// uniform bool normal_set;
// uniform bool specular_set;
// uniform bool ambient_set;
// uniform bool shininess_set;


uniform vec3 viewPos;

out vec4 FragColor;

in VS_OUT
{
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_tex.value, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 compute_directional_light(LightSource light, vec3 normal, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess);
vec3 compute_point_light(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess);
vec3 compute_spot_light(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess);

void main()
{
    // normal

    vec3 norm = normalize(fs_in.Normal);
    if (normal_tex.set > 0)
    {
        norm = getNormalFromMap();
    }

    vec3 diffuse = diffuse_tex.set * texture(diffuse_tex.value, fs_in.TexCoords).rgb - (diffuse_tex.set - 1) * diffuse_tex.default_value;
    vec3 ambient = ambient_tex.set * texture(ambient_tex.value, fs_in.TexCoords).rgb - (ambient_tex.set - 1) * ambient_tex.default_value;
    vec3 specular = specular_tex.set * texture(specular_tex.value, fs_in.TexCoords).rgb - (specular_tex.set - 1) * specular_tex.default_value;
    float shininess = shininess_tex.set * texture(shininess_tex.value, fs_in.TexCoords).r - (shininess_tex.set - 1) * shininess_tex.default_value;

    // diffuse
    // vec3 diffuse = diffuse_default;
    // if (diffuse_set)
    // {
    //     diffuse = texture(diffuse_tex, fs_in.TexCoords).rgb;
    // }

    // ambient
    // vec3 ambient  = ambient_default;
    // if (ambient_set)
    // {
    //     ambient = texture(ambient_tex, fs_in.TexCoords).rgb;
    // }

    // specular
    // vec3 specular = specular_default;
    // if (specular_set)
    // {
    //     specular = texture(specular_tex, fs_in.TexCoords).rgb;
    // }

    // shininess
    // float shininess = shininess_default;
    // if (shininess_set)
    // {
    //     shininess = texture(shininess_tex, fs_in.TexCoords).r;
    // }

    vec3 viewDir = normalize(viewPos - fs_in.WorldPos);

    vec3 result = vec3(0,0,0);

     for(int i = 0; i < lights_count; i++)
    {
        if (lights_sources[i].type == LIGHT_DIRECTIONAL)
        {
            result += compute_directional_light(lights_sources[i], norm, viewDir, diffuse, specular, shininess);
        }
        else if (lights_sources[i].type == LIGHT_POINT)
        {
            result += compute_point_light(lights_sources[i], norm, fs_in.WorldPos, viewDir, diffuse, specular, shininess);
        }
        else if (lights_sources[i].type == LIGHT_SPOT)
        {
            result += compute_spot_light(lights_sources[i], norm, fs_in.WorldPos, viewDir, diffuse, specular, shininess);
        }
    }

    // vec3 result = compute_directional_light(directional_light, norm, viewDir, diffuse, specular, shininess);
    // phase 2: point lights
    //  for(int i = 0; i < lights_count; i++)
    //      result += compute_point_light(PointLights[i], norm, fs_in.WorldPos, viewDir, diffuse, specular, shininess);
    // phase 3: spot light
    // result += compute_spot_light(spot_light, norm, fs_in.WorldPos, viewDir, diffuse, specular, shininess);
    
    FragColor = vec4(result, 1.0);
}


vec3 compute_directional_light(LightSource light, vec3 normal, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.color * material_diffuse;
    vec3 diffuse = light.color * diff * material_diffuse;
    vec3 specular = light.color * spec * material_specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 compute_point_light(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess)
{
    // if (!light.is_on)
    // {
    //     return vec3(0,0,0);
    // }
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.color * material_diffuse;
    vec3 diffuse = light.color * diff * material_diffuse;
    vec3 specular = light.color * spec * material_specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 compute_spot_light(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffuse, vec3 material_specular, float shininess)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.color * material_diffuse;
    vec3 diffuse = light.color * diff * material_diffuse;
    vec3 specular = light.color * spec * material_specular;
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}