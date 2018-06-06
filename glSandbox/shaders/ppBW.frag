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
    const float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}
