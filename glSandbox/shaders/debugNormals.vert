#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform bool viewSpace;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 n;
void main()
{
	if(viewSpace)
		n = mat3(transpose(inverse(view * model))) * aNormal;
	else
		n = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}