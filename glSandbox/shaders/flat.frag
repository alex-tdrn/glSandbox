#version 420 core

in GS_OUT
{
	vec4 color;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = fs_in.color;
}