#version 420 core
uniform sampler2D screenTexture;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	float scale = length(fs_in.textureCoordinates - vec2(0.5f, 0.5f));
	float intensity = 0.002f;
	vec2 offset = vec2(intensity, intensity) * scale;
	FragColor.r = texture(screenTexture, fs_in.textureCoordinates).r;
	FragColor.g = texture(screenTexture, fs_in.textureCoordinates + offset).g;
	FragColor.b = texture(screenTexture, fs_in.textureCoordinates - offset).b;
}
