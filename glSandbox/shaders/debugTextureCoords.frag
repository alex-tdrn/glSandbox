#version 420 core

in vec2 t;
out vec4 FragColor;

void main()
{
	FragColor = vec4(t, 0.0f, 1.0f);
}
