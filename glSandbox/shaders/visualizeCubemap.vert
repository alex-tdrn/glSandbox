#version 420 core

layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model;

out VS_OUT{
	vec3 textureCoordinates;
} vs_out;

void main()
{
	vs_out.textureCoordinates = position;
	gl_Position = projection * model * vec4(position, 1.0f);
}
