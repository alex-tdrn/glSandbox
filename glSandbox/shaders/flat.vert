#version 420 core
layout (location = 0) in vec3 position;
layout (location = 3) in vec2 textureCoordinates;

out VS_OUT
{
	vec2 textureCoordinates;
} vs_out;

void main()
{
	vs_out.textureCoordinates = textureCoordinates;
	gl_Position =  vec4(position, 1.0f);
}
