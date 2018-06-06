#version 420 core
uniform sampler2D screenTexture;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = texture(screenTexture, fs_in.textureCoordinates);
}
