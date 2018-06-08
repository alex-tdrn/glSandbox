#version 420 core
uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform bool perChannel;
uniform float n1;
uniform vec3 n1RGB;
uniform float n2;
uniform vec3 n2RGB;

in VS_OUT
{
	vec3 position;
	vec3 normal;
} fs_in;

out vec4 FragColor;

void main()
{             
    const vec3 I = normalize(fs_in.position - cameraPos);
	if(perChannel)
	{
		const vec3 Rr = refract(I, normalize(fs_in.normal), n1RGB.r / n2RGB.r);
		const vec3 Rg = refract(I, normalize(fs_in.normal), n1RGB.g / n2RGB.g);
		const vec3 Rb = refract(I, normalize(fs_in.normal), n1RGB.b / n2RGB.b);
		FragColor = vec4(texture(skybox, Rr).r, texture(skybox, Rg).g, texture(skybox, Rb).b, 1.0);
	}
	else
	{
		const vec3 R = refract(I, normalize(fs_in.normal), n1 / n2);
		FragColor = vec4(texture(skybox, R).rgb, 1.0);
	}
}