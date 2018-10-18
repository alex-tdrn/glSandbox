#version 420 core
#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct DirLight
{
	vec3 color;
	float intensity;
	vec3 direction;
	sampler2DShadow shadowMap;
};
struct PointLight
{
	vec3 color;
	float intensity;
	vec3 position;
	vec3 worldPosition;
	samplerCubeShadow shadowMap;
};
struct SpotLight
{
	vec3 color;
	float intensity;
	vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
	sampler2DShadow shadowMap;
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
uniform bool shadowMappingEnabled;
uniform float shadowMappingBiasMin;
uniform float shadowMappingBiasMax;
uniform int shadowMappingPCFSamples;
uniform float shadowMappingPCFRadius;
uniform float shadowMappingOmniFarPlane;
uniform bool shadowMappingPCFEarlyExit;

in VS_OUT
{
	vec3 position;
	vec3 worldPosition;
	vec3 normal;
	mat3 TBN;
	vec2 textureCoordinates;
	vec4 positionLightSpaceD[MAX_DIR_LIGHTS];
	vec4 positionLightSpaceS[MAX_SPOT_LIGHTS];
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

vec3 calculateAmbientLight();
vec3 calculateDirLight(DirLight light, vec4 positionInLightSpace);
vec3 calculatePointLight(PointLight light);
vec3 calculateSpotLight(SpotLight light, vec4 positionInLightSpace);

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

	vec3 result = calculateAmbientLight() * occlusion + emission;
	for(int i = 0; i < nDirLights; i++)
		result += calculateDirLight(dirLights[i], fs_in.positionLightSpaceD[i]);
	for(int i = 0; i < nPointLights; i++)
		result += calculatePointLight(pointLights[i]);
	for(int i = 0; i < nSpotLights; i++)
		result += calculateSpotLight(spotLights[i], fs_in.positionLightSpaceS[i]);

	result /= result + vec3(1.0f);//tonemap
	//result = pow(result, vec3(1.0f / 2.2f));//gamma
	FragColor = vec4(result , 1.0f);
}

vec3 BRDF(vec3 lightDirection);
float calculateShadowBias(vec3 lightDirection);
float calculateShadowFactorPCF(vec4 coords, sampler2DShadow shadowMap, float bias);
float calculateShadowFactorPCF(vec3 coords, samplerCubeShadow shadowMap, float bias);

vec3 calculateAmbientLight()
{
	return ambientStrength * ambientColor * baseColor;	
}

vec3 calculateDirLight(DirLight light, vec4 positionInLightSpace)
{
	vec3 lightDirection = normalize(-light.direction);
	float fragmentOrientationToLight = max(dot(normal, lightDirection), 0.0f);
	float shadowFactor = 1.0f;
	if(shadowMappingEnabled && fragmentOrientationToLight > 0.0f)
	{
		shadowFactor = calculateShadowFactorPCF(positionInLightSpace, 
		light.shadowMap, calculateShadowBias(lightDirection));
	}
	vec3 radiance = light.color * light.intensity * shadowFactor;
	return BRDF(lightDirection) * radiance * fragmentOrientationToLight;
}

vec3 calculatePointLight(PointLight light)
{
	float distance = length(fs_in.position - light.position);
	float attenuation = 1.0 / (distance * distance);
	vec3 lightDirection = normalize(-fs_in.position + light.position);
	float fragmentOrientationToLight = max(dot(normal, lightDirection), 0.0f);
	float shadowFactor = 1.0f;
	if(shadowMappingEnabled && fragmentOrientationToLight > 0.0f)
	{
		shadowFactor = calculateShadowFactorPCF(fs_in.worldPosition - light.worldPosition,
		light.shadowMap, calculateShadowBias(lightDirection));
	}
	vec3 radiance = light.color * light.intensity * attenuation * shadowFactor;
	return BRDF(lightDirection) * radiance * fragmentOrientationToLight;
}

vec3 calculateSpotLight(SpotLight light, vec4 positionInLightSpace)
{
	vec3 lightDirection = normalize(light.position - fs_in.position);
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / (distance * distance);
	attenuation *= clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
	float fragmentOrientationToLight = max(dot(normal, lightDirection), 0.0f);
	float shadowFactor = 1.0f;
	if(shadowMappingEnabled && fragmentOrientationToLight > 0.0f)
	{
		shadowFactor = calculateShadowFactorPCF(positionInLightSpace,
		light.shadowMap, calculateShadowBias(lightDirection));
	}
	vec3 radiance = light.color * light.intensity * attenuation * shadowFactor;
	return BRDF(lightDirection) * radiance * fragmentOrientationToLight;
}

//Lighting functions
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

//Shadowing sampling functions
float calculateShadowBias(vec3 lightDirection)
{
	return max(shadowMappingBiasMax * (1.0 - dot(normal, lightDirection)), shadowMappingBiasMin);
}

float sampleShadow(vec3 coords, sampler2DShadow shadowMap, float bias)
{
	return texture(shadowMap, vec3(coords.xy, coords.z - bias));
}

float sampleShadow(vec3 coords, samplerCubeShadow shadowMap, float bias)
{
	return texture(shadowMap, vec4(coords.xyz, length(coords) / shadowMappingOmniFarPlane - bias));
}

float calculateShadowFactorPCF(vec4 coords, sampler2DShadow shadowMap, float bias)
{
	coords /= coords.w;
	coords = coords * 0.5 + 0.5;
	float shadow = sampleShadow(coords.xyz, shadowMap, bias);
	if(shadowMappingPCFSamples == 0)
		return shadow;
	vec2 texelSize = shadowMappingPCFRadius / textureSize(shadowMap, 0);
	//early exit test
	vec3 corners[4] = {
		vec3(-texelSize * shadowMappingPCFSamples, 0.0f),
		vec3(vec2(-texelSize.x, +texelSize.y) * shadowMappingPCFSamples, 0.0f),
		vec3(vec2(+texelSize.x, -texelSize.y) * shadowMappingPCFSamples, 0.0f),
		vec3(+texelSize * shadowMappingPCFSamples, 0.0f)
	};
	for(int i = 0; i < 4; i++)
		shadow += sampleShadow(coords.xyz + corners[i], shadowMap, bias);
	if(shadowMappingPCFEarlyExit && (shadow == 0.0f || shadow == 5.0f))
		return shadow / 5;

	for(int x = -shadowMappingPCFSamples; x <= shadowMappingPCFSamples; ++x)
	{
		for(int y = -shadowMappingPCFSamples; y <= shadowMappingPCFSamples; ++y)
		{
			if(x == 0 && y == 0 ) 
				continue;
			vec3 offset = vec3(vec2(x, y) * texelSize, 0.0f);
			if(offset == corners[0]
			|| offset == corners[1]
			|| offset == corners[2]
			|| offset == corners[3])
				continue;
			shadow += sampleShadow(coords.xyz + offset, shadowMap, bias);
		}    
	}
	float sampleCount = shadowMappingPCFSamples * 2;
	sampleCount += 1;
	sampleCount *= sampleCount;
	return shadow / sampleCount;
}

float calculateShadowFactorPCF(vec3 coords, samplerCubeShadow shadowMap, float bias)
{
	vec3 aux = vec3(0.0f, 1.0f, 0.0f);//use as helper vector to find base vectors
	if(dot(coords, aux) == 1.0f)
		aux = vec3(0.0f, 0.0f, 1.0f);
	vec3 xBase = cross(aux, coords);
	vec3 yBase = cross(aux, xBase);
	//now do normal pcf sampling using x, y as offset base
	float shadow = sampleShadow(coords, shadowMap, bias);
	if(shadowMappingPCFSamples == 0)
		return shadow;
	vec2 texelSize = shadowMappingPCFRadius / textureSize(shadowMap, 0);
	//early exit test
	vec3 corners[4] = {
		(-texelSize.x * xBase -texelSize.y * yBase) * shadowMappingPCFSamples,
		(-texelSize.x * xBase +texelSize.y * yBase) * shadowMappingPCFSamples,
		(+texelSize.x * xBase -texelSize.y * yBase) * shadowMappingPCFSamples,
		(+texelSize.x * xBase +texelSize.y * yBase) * shadowMappingPCFSamples
	};
	for(int i = 0; i < 4; i++)
		shadow += sampleShadow(coords + corners[i], shadowMap, bias);
	if(shadowMappingPCFEarlyExit && (shadow == 0.0f || shadow == 5.0f))
		return shadow / 5;

	for(int x = -shadowMappingPCFSamples; x <= shadowMappingPCFSamples; ++x)
	{
		for(int y = -shadowMappingPCFSamples; y <= shadowMappingPCFSamples; ++y)
		{
			if(x == 0 && y == 0 ) 
				continue;
			vec3 offset = texelSize.x * xBase * x + texelSize.y * yBase * y;
			if(offset == corners[0]
			|| offset == corners[1]
			|| offset == corners[2]
			|| offset == corners[3])
				continue;
			shadow += sampleShadow(coords + offset, shadowMap, bias);
		}    
	}
	float sampleCount = shadowMappingPCFSamples * 2;
	sampleCount += 1;
	sampleCount *= sampleCount;
	return shadow / sampleCount;
}
