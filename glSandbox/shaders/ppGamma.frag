#version 420 core
uniform sampler2D screenTexture;
uniform float e;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor.rgb = pow(texture(screenTexture, fs_in.textureCoordinates).rgb, vec3(1 / e));
}
