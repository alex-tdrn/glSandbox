#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};

layout (location = 0) in vec3 position;

out VS_OUT
{
	vec3 textureCoordinates;
} vs_out;

void main()
{
    vs_out.textureCoordinates = position;
    gl_Position = (projection * mat4(mat3(view)) * vec4(position, 1.0)).xyww;
}  