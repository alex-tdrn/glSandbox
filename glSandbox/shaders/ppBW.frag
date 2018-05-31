#version 330 core

uniform sampler2D screenTexture;
in vec2 texCoord;
out vec4 FragColor;

void main()
{
	FragColor = texture(screenTexture, texCoord);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}
