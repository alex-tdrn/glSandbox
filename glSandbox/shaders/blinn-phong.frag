#version 460 core
#define MAX_DIR_LIGHTS 256
#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256

struct Material
{
	bool overrideDiffuse;
	vec3 overrideDiffuseColor;
	bool hasDiffuseMap;
	sampler2D diffuseMap;

	bool overrideSpecular;
	vec3 overrideSpecularColor;
	bool hasSpecularMap;
	sampler2D specularMap;
	float shininess;

	bool hasOpacityMap;
	sampler2D opacityMap;
};

struct DirLight
{
	vec3 color;
	float intensity;
	vec3 direction;
};
struct PointLight
{
	vec3 color;
	float intensity;
	vec3 position;
};
struct SpotLight
{
	vec3 color;
	float intensity;
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

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

vec3 viewDirection = normalize(-fs_in.position);
vec3 diffuseColor;
vec3 specularColor;
vec3 normal = normalize(fs_in.normal);
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);
vec3 ambient();

void main()
{	
	if(material.hasOpacityMap)
	{
		float opacity = texture(material.opacityMap, fs_in.textureCoordinates).r;
		if(opacity <= 0.1f)
			discard;
	}
	if(!material.overrideDiffuse && material.hasDiffuseMap)
		diffuseColor = vec3(texture(material.diffuseMap, fs_in.textureCoordinates));
	else if(material.overrideDiffuse)
		diffuseColor = material.overrideDiffuseColor;
	else
		diffuseColor = vec3(1.0f);

	if(!material.overrideSpecular && material.hasSpecularMap)
		specularColor = vec3(texture(material.specularMap, fs_in.textureCoordinates));
	else if(material.overrideSpecular)
		specularColor = material.overrideSpecularColor;
	else
		specularColor = vec3(1.0f);

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
	vec3 halfwayDir = normalize(lightDirection + viewDirection);
	float m = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	return m * specularColor;
}

vec3 calcDirLight(DirLight light)
{
	vec3 lightDirection = normalize(-light.direction);

    return light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / (distance * distance);
	vec3 lightDirection = normalize(light.position - fs_in.position);

	return attenuation * light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - fs_in.position);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

	return intensity * light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}