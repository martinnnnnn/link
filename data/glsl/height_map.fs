#version 420 core

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform sampler2D tex_sampler;

out vec4 FragColor;

in VS_OUT
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    vec3 result = ambient + diffuse;

    //FragColor = vec4(result, 1.0);  
    FragColor = vec4(result, 1.0) * vec3(0.3, 0.3, 0.3, 1);
    // FragColor = vec4(result, 1.0) * texture(tex_sampler, fs_in.TexCoord);

	// FragColor = texture(tex_sampler, fs_in.TexCoord);
	// FragColor = vec4(fs_in.Normal, 1.0);
} 
