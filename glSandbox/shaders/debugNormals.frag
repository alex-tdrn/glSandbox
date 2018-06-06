#version 420 core

in vec3 n;
out vec4 FragColor;

void main()
{
	FragColor = vec4(normalize(n), 1.0f);
}
