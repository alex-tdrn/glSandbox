#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoordinates;

out VS_OUT
{
	vec2 textureCoordinates;
} vs_out;

void main()
{
	vs_out.textureCoordinates = textureCoordinates;
	gl_Position = vec4(position, 0.0f, 1.0f);
}