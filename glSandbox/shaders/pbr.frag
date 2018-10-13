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
	bool normalMapExists;
	sampler2D normalMap;

	bool occlusionMapExists;
	sampler2D occlusionMap;

	bool emissiveMapExists;
	sampler2D emissiveMap;
	vec3 emissiveFactor;

	bool baseColorMapExists;
	sampler2D baseColorMap;
	vec4 baseColorFactor;

	bool metallicRoughnessMapExists;
	sampler2D metallicRoughnessMap;
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

uniform sampler2D shadowMap;

in VS_OUT
{
	vec3 position;
	vec3 normal;
	mat3 TBN;
	vec2 textureCoordinates;
	vec4 positionLightSpace;
} fs_in;

out vec4 FragColor;

const float PI = 3.14159265359;
vec3 viewDirection = normalize(-fs_in.position);
vec3 normal;
vec3 baseColor = vec3(material.baseColorFactor);
float metalness = material.metallicFactor;
float roughness = material.roughnessFactor;
vec3 occlusion = vec3(1.0f);
vec3 emission = material.emissiveFactor;
vec3 F0 = vec3(0.04);

float calcShadow();
vec3 calcAmbientLight();
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);
vec3 BRDF(vec3 lightDirection);

void main()
{
	if(material.normalMapExists)
	{
		normal = texture(material.normalMap, fs_in.textureCoordinates).xyz;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(fs_in.TBN * normal);
	}
	else
	{
		normal = normalize(fs_in.normal);
	}
	if(material.occlusionMapExists)
	{
		occlusion *= texture(material.occlusionMap, fs_in.textureCoordinates).r;
	}
	if(material.emissiveMapExists)
	{
		emission *= texture(material.emissiveMap, fs_in.textureCoordinates).rgb;
	}
	if(material.baseColorMapExists)
	{
		baseColor *= texture(material.baseColorMap, fs_in.textureCoordinates).rgb;
	}
	if(material.metallicRoughnessMapExists)
	{
		vec4 mr = texture(material.metallicRoughnessMap, fs_in.textureCoordinates);
		roughness *= mr.g;
		metalness *= mr.b;
	}

	F0 = mix(F0, baseColor, metalness);

	vec3 result = calcAmbientLight() * occlusion + emission;
	for(int i = 0; i < nDirLights; i++)
		result += calcDirLight(dirLights[i]) * calcShadow();
	for(int i = 0; i < nPointLights; i++)
		result += calcPointLight(pointLights[i]);
	for(int i = 0; i < nSpotLights; i++)
		result += calcSpotLight(spotLights[i]);

	result /= result + vec3(1.0f);//tonemap
	//result = pow(result, vec3(1.0f / 2.2f));//gamma
	FragColor = vec4(result , 1.0f);
}

float calcShadow()
{
	vec3 projCoordinates = fs_in.positionLightSpace.xyz / fs_in.positionLightSpace.w;
	projCoordinates = projCoordinates * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoordinates.xy).r;
	float currentDepth = projCoordinates.z;
	float bias = 0.005f;
	return currentDepth - bias > closestDepth ? 0.0f : 1.0f;
}
vec3 calcAmbientLight()
{
	return ambientStrength * ambientColor * baseColor;	
}

vec3 calcDirLight(DirLight light)
{
	vec3 lightDirection = normalize(-light.direction);
    vec3 radiance = light.color * light.intensity;
	return BRDF(lightDirection) * radiance * max(dot(normal, lightDirection), 0.0f);
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / (distance * distance);
	vec3 lightDirection = normalize(light.position - fs_in.position);
	vec3 radiance = light.color * light.intensity * attenuation;
	return BRDF(lightDirection) * radiance * max(dot(normal, lightDirection), 0.0f);
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - fs_in.position);
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / (distance * distance);
	attenuation *= clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
	vec3 radiance = light.color * light.intensity * attenuation;
	return BRDF(lightDirection) * radiance * max(dot(normal, lightDirection), 0.0f);
}

float DistributionGGX(vec3 halfwayVector)
{
	float r4 = roughness * roughness;
	r4 *= r4;
	float cosAlpha2 = max(dot(normal, halfwayVector), 0.0f);
	cosAlpha2 *= cosAlpha2;
	float denominator = cosAlpha2 * (r4 - 1.0f) + 1.0f;
	denominator *= PI * denominator;

	return r4 / denominator;
}

float GeometrySchlickGGX(float cosAlpha)
{
	float k = roughness + 1.0f;
	k = k * k / 8.0f;
	return cosAlpha / (cosAlpha * (1.0f - k) + k);
}

float GeometrySmith(vec3 lightDirection)
{
	float ggx1 = GeometrySchlickGGX(max(dot(normal, lightDirection), 0.0f));
	float ggx2 = GeometrySchlickGGX(max(dot(normal, viewDirection), 0.0f));
	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosAlpha)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosAlpha, 5.0f);
}

vec3 BRDF(vec3 lightDirection)
{
	vec3 halfwayVector = normalize(viewDirection + lightDirection);

	float D = DistributionGGX(halfwayVector);
	float G = GeometrySmith(lightDirection);
	vec3 F = FresnelSchlick(max(dot(halfwayVector, viewDirection), 0.0f));

	float denominator = 4 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.001;
	vec3 specular = (D * G * F) / denominator;
	vec3 kD = vec3(1.0f) - F;
	kD *= 1.0f - metalness;
	vec3 diffuse = kD * baseColor / PI;

	return diffuse + specular;
}

