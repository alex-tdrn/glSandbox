#version 420 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
void main()
{
    TexCoords = aPos;
    gl_Position = (projection * mat4(mat3(view)) * vec4(aPos, 1.0)).xyww;
}  