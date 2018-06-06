#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform bool viewSpace;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
	vec3 normal;
	vec3 clipSpaceNormal;
} vs_out;

void main()
{
	if(viewSpace)
		vs_out.normal = mat3(transpose(inverse(view * model))) * normal;
	else
		vs_out.normal = mat3(transpose(inverse(model))) * normal;
	vs_out.clipSpaceNormal = normalize(mat3(projection * transpose(inverse(view * model))) * normal);
	gl_Position = projection * view * model * vec4(position, 1.0f);
}