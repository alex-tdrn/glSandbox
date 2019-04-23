#version 420 core
uniform sampler2D screenTexture;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	vec3 result = texture(screenTexture, fs_in.textureCoordinates).rgb;
	result /= result + vec3(1.0f);//tonemap
	result = pow(result, vec3(1.0f / 2.2f));//gamma
	FragColor = vec4(result, 1.0f);
}
