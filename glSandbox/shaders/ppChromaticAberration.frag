#version 420 core
uniform sampler2D screenTexture;
uniform float intensity;
uniform vec2 offsetR;
uniform vec2 offsetG;
uniform vec2 offsetB;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor.r = texture(screenTexture, fs_in.textureCoordinates + offsetR * intensity).r;
	FragColor.g = texture(screenTexture, fs_in.textureCoordinates + offsetG * intensity).g;
	FragColor.b = texture(screenTexture, fs_in.textureCoordinates + offsetB * intensity).b;
}
