#version 420 core
uniform mat4 lightSpace;
uniform mat4 model;

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = lightSpace * model * vec4(position, 1.0f);
}