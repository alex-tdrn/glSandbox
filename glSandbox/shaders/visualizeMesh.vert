#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinates;

out VS_OUT{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 textureCoordinates;
} vs_out;

void main()
{
	vs_out.position = position;
	vs_out.normal = abs(normal);
	vs_out.tangent = abs(tangent.xyz * tangent.w);
	vs_out.textureCoordinates = textureCoordinates;

	gl_Position = vec4(position, 1.0f);
}