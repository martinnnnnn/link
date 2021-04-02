#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec3 normal;


out VS_OUT
{
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;


layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vs_out.WorldPos = vec3(model * vec4(position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * normal;
    vs_out.TexCoords = tex_coords;
}