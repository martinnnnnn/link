#version 420 core

uniform vec3 color;

out vec4 FragColor;

in VS_OUT
{
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


void main()
{
    FragColor = vec4(fs_in.Normal, 1.0);
}