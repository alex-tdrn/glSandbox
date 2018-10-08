#version 420 core
#define MAX_DIR_LIGHTS 32
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

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

struct Material
{
	bool baseColorMapExists;
	sampler2D baseColorMap;
	vec4 baseColorFactor;

	bool metallicRoughnessMapExists;
	sampler2D metallichRoughnessMap;
	float metallicFactor;
	float roughnessFactor;
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

vec3 calcAmbientLight();
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);

void main()
{
	vec3 baseColor = vec3(material.baseColorFactor);
	if(material.baseColorMapExists)
		baseColor *= vec3(texture(material.baseColorMap, fs_in.textureCoordinates));

	float metalness = material.metallicFactor;
	float roughness = material.roughnessFactor;
	if(material.metallicRoughnessMapExists)
	{
		vec4 mr = texture(material.metallichRoughnessMap, fs_in.textureCoordinates);
		metalness *= mr.r;
		roughness *= mr.g;
	}

	vec3 result = calcAmbientLight();
	for(int i = 0; i < nDirLights; i++)
		result += calcDirLight(dirLights[i]);
	for(int i = 0; i < nPointLights; i++)
		result += calcPointLight(pointLights[i]);
	for(int i = 0; i < nSpotLights; i++)
		result += calcSpotLight(spotLights[i]);

	//FragColor = vec4(result , 1.0f);
	FragColor = vec4(baseColor, 1.0f);
}

vec3 calcAmbientLight()
{
	return vec3(1.0f, 1.0f, 1.0f);

	//return ambientStrength * ambientColor * diffuseColor;	
}

vec3 calcDirLight(DirLight light)
{
	vec3 lightDirection = normalize(-light.direction);
	return vec3(1.0f, 1.0f, 1.0f);
    //return light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / (distance * distance);
	vec3 lightDirection = normalize(light.position - fs_in.position);
	return vec3(1.0f, 1.0f, 1.0f);

	//return attenuation * light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - fs_in.position);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
	return vec3(1.0f, 1.0f, 1.0f);

	//return intensity * light.color * light.intensity * (diffuse(lightDirection) + specular(lightDirection));
}
