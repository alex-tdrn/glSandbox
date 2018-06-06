#version 420 core

in VS_OUT
{
	vec3 normal;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = vec4(normalize(fs_in.normal), 1.0f);
}
