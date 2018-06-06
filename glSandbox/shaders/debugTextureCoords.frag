#version 420 core

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = vec4(fs_in.textureCoordinates, 0.0f, 1.0f);
}
