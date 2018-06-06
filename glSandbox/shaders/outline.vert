#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

void main()
{
	gl_Position = projection * view * model * vec4(position + 0.1f * normal, 1.0f);
}