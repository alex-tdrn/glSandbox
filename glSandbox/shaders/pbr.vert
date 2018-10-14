#version 420 core
#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;
uniform int nDirLights;
uniform int nPointLights;
uniform int nSpotLights;
uniform mat4 lightSpacesD[MAX_DIR_LIGHTS];
uniform mat4 lightSpacesS[MAX_SPOT_LIGHTS];

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinates;


out VS_OUT
{
	vec3 position;
	vec3 worldPosition;
	vec3 normal;
	mat3 TBN;
	vec2 textureCoordinates;
	vec4 positionLightSpaceD[MAX_DIR_LIGHTS];
	vec4 positionLightSpaceS[MAX_SPOT_LIGHTS];
} vs_out;

void main()
{
	vs_out.worldPosition = vec3(model * vec4(position, 1.0f));
	vs_out.position = vec3(view * vec4(vs_out.worldPosition, 1.0f));
	for(int i = 0; i < nDirLights; i++)
		vs_out.positionLightSpaceD[i] = lightSpacesD[i] * model * vec4(position, 1.0f);
	for(int i = 0; i < nSpotLights; i++)
		vs_out.positionLightSpaceS[i] = lightSpacesS[i] * model * vec4(position, 1.0f);
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vec3 t = normalMatrix * tangent.xyz;
	vec3 b = normalMatrix * cross(normal, tangent.xyz) * tangent.w;
	vs_out.normal = normalMatrix * normal;
	vs_out.TBN = mat3(t, b, vs_out.normal);
	vs_out.textureCoordinates = textureCoordinates;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}