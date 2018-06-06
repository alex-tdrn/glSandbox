#version 420 core
uniform samplerCube skybox;
uniform vec3 cameraPos;

in VS_OUT
{
	vec3 position;
	vec3 normal;
} fs_in;

out vec4 FragColor;

void main()
{             
    const vec3 I = normalize(fs_in.position - cameraPos);
    const vec3 R = reflect(I, normalize(fs_in.normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}