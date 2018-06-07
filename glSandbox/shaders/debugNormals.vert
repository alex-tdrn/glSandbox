#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform bool viewSpace;
uniform bool faceNormals;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
	vec3 normal;
} vs_out;

void main()
{
	if(!faceNormals)
	{
		if(viewSpace)
			vs_out.normal = mat3(transpose(inverse(view * model))) * normal;
		else
			vs_out.normal = mat3(transpose(inverse(model))) * normal;
	}
	gl_Position =  vec4(position, 1.0f);
}