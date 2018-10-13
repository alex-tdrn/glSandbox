#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform mat4 lightSpace;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinates;


out VS_OUT
{
	vec3 position;
	vec3 normal;
	mat3 TBN;
	vec2 textureCoordinates;
	vec4 positionLightSpace;
} vs_out;

void main()
{
	vs_out.position = vec3(view * model * vec4(position, 1.0f));
	vs_out.positionLightSpace = lightSpace * model * vec4(position, 1.0f);
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vec3 t = normalMatrix * tangent.xyz;
	vec3 b = normalMatrix * cross(normal, tangent.xyz) * tangent.w;
	vs_out.normal = normalMatrix * normal;
	vs_out.TBN = mat3(t, b, vs_out.normal);
	vs_out.textureCoordinates = textureCoordinates;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}