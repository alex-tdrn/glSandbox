#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 textureCoordinates;

out VS_OUT
{
	vec2 textureCoordinates;
} vs_out;

void main()
{
	vs_out.textureCoordinates = textureCoordinates;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}