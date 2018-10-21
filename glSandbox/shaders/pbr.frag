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
uniform bool shadowMappingUsePoisson;
uniform int shadowMappingPoissonVariant;
uniform int shadowMappingSamples;
uniform float shadowMappingRadius[2];
uniform bool shadowMappingEarlyExit;
uniform float shadowMappingOmniFarPlane;
uniform float cameraFarPlane;

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
const float shadowSamplingRadius = 
	shadowMappingRadius[0] + (length(fs_in.position) / cameraFarPlane)
	* (shadowMappingRadius[1] - shadowMappingRadius[0]);

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

	//result /= result + vec3(1.0f);//tonemap
	//result = pow(result, vec3(1.0f / 2.2f));//gamma
	FragColor = vec4(result , 1.0f);
}

vec3 BRDF(vec3 lightDirection);
float calculateShadowBias(vec3 lightDirection);
float calculateShadowFactorPCF(vec4 coords, sampler2DShadow shadowMap, float bias);
float calculateShadowFactorPCF(vec3 coords, samplerCubeShadow shadowMap, float bias);
float calculateShadowFactorPoisson(vec4 coords, sampler2DShadow shadowMap, float bias);
float calculateShadowFactorPoisson(vec3 coords, samplerCubeShadow shadowMap, float bias);

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
		if(shadowMappingUsePoisson)
			shadowFactor = calculateShadowFactorPoisson(positionInLightSpace, 
			light.shadowMap, calculateShadowBias(lightDirection));
		else
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
		if(shadowMappingUsePoisson)
			shadowFactor = calculateShadowFactorPoisson(fs_in.worldPosition - light.worldPosition,
			light.shadowMap, calculateShadowBias(lightDirection));
		else
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
		if(shadowMappingUsePoisson)
			shadowFactor = calculateShadowFactorPoisson(positionInLightSpace, 
			light.shadowMap, calculateShadowBias(lightDirection));
		else
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

float sampleShadow(vec4 coords, samplerCubeShadow shadowMap)
{
	return texture(shadowMap, coords);
}

float calculateShadowFactorPCF(vec4 coords, sampler2DShadow shadowMap, float bias)
{
	coords /= coords.w;
	coords = coords * 0.5 + 0.5;
	float shadow = sampleShadow(coords.xyz, shadowMap, bias);
	if(shadowMappingSamples == 0)
		return shadow;
	vec2 texelSize = shadowSamplingRadius / textureSize(shadowMap, 0);
	//early exit test
	vec3 corners[4] = {
		vec3(-texelSize * shadowMappingSamples, 0.0f),
		vec3(vec2(-texelSize.x, +texelSize.y) * shadowMappingSamples, 0.0f),
		vec3(vec2(+texelSize.x, -texelSize.y) * shadowMappingSamples, 0.0f),
		vec3(+texelSize * shadowMappingSamples, 0.0f)
	};
	for(int i = 0; i < 4; i++)
		shadow += sampleShadow(coords.xyz + corners[i], shadowMap, bias);
	if(shadowMappingEarlyExit && (shadow == 0.0f || shadow == 5.0f))
		return shadow / 5;

	for(int x = -shadowMappingSamples; x <= shadowMappingSamples; ++x)
	{
		for(int y = -shadowMappingSamples; y <= shadowMappingSamples; ++y)
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
	float sampleCount = shadowMappingSamples * 2;
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
	float currentDepth = length(coords) / shadowMappingOmniFarPlane - bias;
	float shadow = sampleShadow(vec4(coords, currentDepth), shadowMap);
	if(shadowMappingSamples == 0)
		return shadow;
	
	vec2 texelSize = shadowSamplingRadius / textureSize(shadowMap, 0);
	//early exit test
	vec3 corners[4] = {
		(-texelSize.x * xBase -texelSize.y * yBase) * shadowMappingSamples,
		(-texelSize.x * xBase +texelSize.y * yBase) * shadowMappingSamples,
		(+texelSize.x * xBase -texelSize.y * yBase) * shadowMappingSamples,
		(+texelSize.x * xBase +texelSize.y * yBase) * shadowMappingSamples
	};
	for(int i = 0; i < 4; i++)
		shadow += sampleShadow(vec4(coords + corners[i], currentDepth), shadowMap);
	if(shadowMappingEarlyExit && (shadow == 0.0f || shadow == 5.0f))
		return shadow / 5;

	for(int x = -shadowMappingSamples; x <= shadowMappingSamples; ++x)
	{
		for(int y = -shadowMappingSamples; y <= shadowMappingSamples; ++y)
		{
			if(x == 0 && y == 0 ) 
				continue;
			vec3 offset = texelSize.x * xBase * x + texelSize.y * yBase * y;
			if(offset == corners[0]
			|| offset == corners[1]
			|| offset == corners[2]
			|| offset == corners[3])
				continue;
			shadow += sampleShadow(vec4(coords + offset, currentDepth), shadowMap);
		}    
	}
	float sampleCount = shadowMappingSamples * 2;
	sampleCount += 1;
	sampleCount *= sampleCount;
	return shadow / sampleCount;
}

uniform vec2 poissonDisk[64] = vec2[64](
	vec2(-0.613392, 0.617481), vec2(0.170019, -0.040254),
	vec2(-0.299417, 0.791925), vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887), vec2(0.421003, 0.027070),
	vec2(-0.817194, -0.271096), vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615), vec2(0.063326, 0.142369),
	vec2(0.203528, 0.214331), vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755), vec2(-0.885922, 0.215369),
	vec2(0.566637, 0.605213), vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352), vec2(0.078707, -0.715323),
	vec2(-0.075838, -0.529344), vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125), vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753), vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366), vec2(0.487472, -0.063082),
	vec2(-0.084078, 0.898312), vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933), vec2(-0.696890, -0.549791),
	vec2(-0.149693, 0.605762), vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878), vec2(-0.016205, -0.872921),
	vec2(0.385784, -0.393902), vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098), vec2(0.634388, -0.049471),
	vec2(-0.688894, 0.007843), vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486), vec2(0.364483, 0.511704),
	vec2(0.034028, 0.325968), vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253), vec2(0.678884, -0.204688),
	vec2(0.001801, 0.780328), vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866), vec2(0.315226, -0.604297),
	vec2(-0.780145, 0.486251), vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430), vec2(-0.494552, -0.711051),
	vec2(0.612476, 0.705252), vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976), vec2(0.504440, 0.372295),
	vec2(0.155736, 0.065157), vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104), vec2(0.789239, -0.419965),
	vec2(-0.545396, 0.538133), vec2(-0.178564, -0.596057)
);

float rand(vec4 seed)
{
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float calculateShadowFactorPoisson(vec4 coords, sampler2DShadow shadowMap, float bias)
{
	coords /= coords.w;
	coords = coords * 0.5 + 0.5;
	vec2 texelSize = shadowSamplingRadius / textureSize(shadowMap, 0);
	float shadow = 0.0f;
	for(int i = 0; i < shadowMappingSamples; i++)
	{
		vec2 offset;
		if(shadowMappingPoissonVariant == 1)//stratified
		{
			offset = poissonDisk[int(shadowMappingSamples * rand(vec4(floor(fs_in.worldPosition * 1000.0f), i)))];
		}
		else if(shadowMappingPoissonVariant == 2)//rotated
		{
			
			float angle = rand(vec4(floor(fs_in.worldPosition * 1000.0f), i)) * 2 * PI;
			float c = cos(angle);
			float s = sin(angle);
			offset = poissonDisk[i];
			offset = vec2(offset.x * c + offset.y * s, offset.x * -s + offset.y * c);
		}
		else//simple
		{
			offset = poissonDisk[i];
		}
		shadow += sampleShadow(coords.xyz + vec3(texelSize * offset, 0.0f), shadowMap, bias);
	}
	return shadow / shadowMappingSamples;
}

float calculateShadowFactorPoisson(vec3 coords, samplerCubeShadow shadowMap, float bias)
{
	vec3 aux = vec3(0.0f, 1.0f, 0.0f);//use as helper vector to find base vectors
	if(dot(coords, aux) == 1.0f)
		aux = vec3(0.0f, 0.0f, 1.0f);
	vec3 xBase = cross(aux, coords);
	vec3 yBase = cross(aux, xBase);
	vec2 texelSize = shadowSamplingRadius / textureSize(shadowMap, 0);
	float currentDepth = length(coords) / shadowMappingOmniFarPlane - bias;
	float shadow = 0.0f;
	for(int i = 0; i < shadowMappingSamples; i++)
	{
		vec3 offset = texelSize.x * poissonDisk[i].x * xBase + texelSize.y * poissonDisk[i].y * yBase;
		shadow += sampleShadow(vec4(coords + offset, currentDepth), shadowMap);
	}
	return shadow / shadowMappingSamples;
}