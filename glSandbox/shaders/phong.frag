#version 330 core
#define MAX_DIR_LIGHTS 256
#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256

struct Material
{
	bool overrideDiffuse;
	vec3 overrideDiffuseColor;
	bool hasDiffuseMap;
	sampler2D diffuseMap;
	vec2 diffuseMapOffset;

	bool overrideSpecular;
	vec3 overrideSpecularColor;
	bool hasSpecularMap;
	sampler2D specularMap;
	vec2 specularMapOffset;
	float shininess;

	bool hasOpacityMap;
	sampler2D opacityMap;
	vec2 opacityMapOffset;
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

in vec2 texCoord;
in vec3 n;
in vec3 fragPos;

out vec4 FragColor;

vec3 viewDirection = normalize(-fragPos);
vec3 diffuseColor;
vec3 specularColor;
vec3 normal = normalize(n);
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);
vec3 ambient();

void main()
{	
	if(material.hasOpacityMap)
	{
		float opacity = texture(material.opacityMap, texCoord + material.opacityMapOffset).r;
		if(opacity <= 0.1f)
			discard;
	}
	if(!material.overrideDiffuse && material.hasDiffuseMap)
		diffuseColor = vec3(texture(material.diffuseMap, texCoord + material.diffuseMapOffset));
	else if(material.overrideDiffuse)
		diffuseColor = material.overrideDiffuseColor;
	else
		diffuseColor = vec3(1.0f);

	if(!material.overrideSpecular && material.hasSpecularMap)
		specularColor = vec3(texture(material.specularMap, texCoord + material.specularMapOffset));
	else if(material.overrideSpecular)
		specularColor = material.overrideSpecularColor;
	else
		specularColor = vec3(0.0f);

	vec3 result = vec3(0,0,0);

	for(int i = 0; i < nDirLights; i++)
		result += calcDirLight(dirLights[i]);

	for(int i = 0; i < nPointLights; i++)
		result += calcPointLight(pointLights[i]);

	for(int i = 0; i < nSpotLights; i++)
		result += calcSpotLight(spotLights[i]);

	FragColor = vec4(ambient() + result , 1.0f);
}
vec3 ambient()
{
	return ambientStrength * ambientColor * diffuseColor;	
}
vec3 diffuse(vec3 lightDirection)
{
	float m = max(dot(normal, lightDirection), 0.0);

    return m * diffuseColor;
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

    return light.color * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	vec3 lightDirection = normalize(light.position - fragPos);

	return attenuation * light.color * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - fragPos);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

	return intensity * light.color * (diffuse(lightDirection) + specular(lightDirection));
}