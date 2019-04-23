#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinates;

uniform mat4 projection;
uniform mat4 ndcTransform;
uniform mat4 model;

out VS_OUT{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 textureCoordinates;
} vs_out;

void main()
{
	vs_out.position = (1.0f + vec3(ndcTransform * vec4(position, 1.0f))) / 2.0f;
	vs_out.normal = abs(normal);
	vs_out.tangent = abs(tangent.xyz * tangent.w);
	vs_out.textureCoordinates = textureCoordinates;

	gl_Position = projection * model * ndcTransform * vec4(position, 1.0f);
}