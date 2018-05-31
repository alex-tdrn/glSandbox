#version 330 core

uniform sampler2D screenTexture;
in vec2 texCoord;
out vec4 FragColor;

void main()
{
	FragColor = 1.0f - texture(screenTexture, texCoord);
}
