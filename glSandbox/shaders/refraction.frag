#version 420 core
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform float n1;
uniform float n2;

in VS_OUT
{
	vec3 position;
	vec3 normal;
} fs_in;

out vec4 FragColor;

void main()
{             
	const float ratio = n1 / n2;
    const vec3 I = normalize(fs_in.position - cameraPos);
    const vec3 R = refract(I, normalize(fs_in.normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}