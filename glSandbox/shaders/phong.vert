#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
out vec2 texCoord;
out vec3 n;
out vec3 fragPos;
void main()
{
	texCoord = aTexCoord;
	n = mat3(transpose(inverse(view * model))) * aNormal;
	fragPos = vec3(view * model * vec4(aPos, 1.0f));
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}