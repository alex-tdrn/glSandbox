#version 330 core
#define MAX_DIR_LIGHTS 256
#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256

struct Material
{
	bool useDiffuseMap;
	bool hasDiffuseMap;
	sampler2D diffuseMap;
	vec3 diffuseColor;

	bool useSpecularMap;
	bool hasSpecularMap;
	sampler2D specularMap;
	vec3 specularColor;
	float shininess;
};

struct DirLight
{
	vec3 color;
	vec3 direction;
};
struct PointLight
{
	vec3 color;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};
struct SpotLight
{
	vec3 color;
	vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
};

uniform Material material;
uniform vec3 ambientColor;
uniform float ambientStrength;
uniform int nDirLights;
uniform int nPointLights;
uniform int nSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uvOffset;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
out vec4 color;

vec3 pos = vec3(view * model * vec4(aPos, 1.0f));
vec3 viewDirection = normalize(-pos);
vec3 diffuseColor;
vec3 specularColor;
vec3 normal = normalize(mat3(transpose(inverse(view * model))) * aNormal);
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);
vec3 ambient();

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	if(material.useDiffuseMap && material.hasDiffuseMap)
		diffuseColor = vec3(texture(material.diffuseMap, texCoord + uvOffset));
	else
		diffuseColor = material.diffuseColor;

	if(material.useSpecularMap && material.hasDiffuseMap)
		specularColor = vec3(texture(material.specularMap, texCoord + uvOffset));
	else
		specularColor = material.specularColor;

	vec3 result = vec3(0,0,0);

	for(int i = 0; i < nDirLights; i++)
		result += calcDirLight(dirLights[i]);

	for(int i = 0; i < nPointLights; i++)
		result += calcPointLight(pointLights[i]);

	for(int i = 0; i < nSpotLights; i++)
		result += calcSpotLight(spotLights[i]);

	color = vec4(ambient() + result , 1.0f);
}
vec3 ambient()
{
	return ambientStrength * ambientColor * diffuseColor;	
}
vec3 diffuse(vec3 lightDirection, vec3 lightColor)
{
	float m = max(dot(normal, lightDirection), 0.0);

    return m * lightColor * diffuseColor;
}

vec3 specular(vec3 lightDirection)
{
	vec3 lightReflection = reflect(-lightDirection, normal);
	float m = pow(max(dot(viewDirection, lightReflection), 0.0), material.shininess);

	return m * specularColor;
}

vec3 calcDirLight(DirLight light)
{
	vec3 lightDirection = normalize(-light.direction);

    return diffuse(lightDirection, light.color) + specular(lightDirection);
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	vec3 lightDirection = normalize(light.position - pos);

	return attenuation * (diffuse(lightDirection, light.color) + specular(lightDirection));
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - pos);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

	return intensity * (diffuse(lightDirection, light.color) + specular(lightDirection));
}