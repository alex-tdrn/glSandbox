#version 420 core
#define MAX_DIR_LIGHTS 256
#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

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
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;

in VS_OUT
{
	vec2 textureCoordinates;
} gs_in[];

out GS_OUT
{
	vec4 color;
} gs_out;

vec3 pos = vec3(view * model * (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0f);
vec3 viewDirection = normalize(-pos);
vec3 diffuseColor;
vec3 specularColor;
vec3 normal = normalize(mat3(transpose(inverse(view * model))) * normalize(cross(vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position), vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position))));
vec2 texCoord = (gs_in[0].textureCoordinates + gs_in[1].textureCoordinates + gs_in[2].textureCoordinates) / 3.0f;
vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);
vec3 ambient();
void main()
{
	if(!material.overrideDiffuse && material.hasDiffuseMap)
		diffuseColor = vec3(texture(material.diffuseMap, texCoord));
	else if(material.overrideDiffuse)
		diffuseColor = material.overrideDiffuseColor;
	else
		diffuseColor = vec3(1.0f);

	if(!material.overrideSpecular && material.hasSpecularMap)
		specularColor = vec3(texture(material.specularMap, texCoord));
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
	for(int i = 0; i < 3; i++)
	{
		gl_Position = projection * view * model * gl_in[i].gl_Position;
		gs_out.color = vec4(ambient() + result , 1.0f);
		EmitVertex();
	}
	EndPrimitive();
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

    return light.intensity * diffuse(lightDirection, light.color) + specular(lightDirection);
}

vec3 calcPointLight(PointLight light)
{
	float distance = length(light.position - pos);
	float attenuation = 1.0 / (distance * distance);
	vec3 lightDirection = normalize(light.position - pos);

	return attenuation * light.intensity * (diffuse(lightDirection, light.color) + specular(lightDirection));
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - pos);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

	return intensity * light.intensity * (diffuse(lightDirection, light.color) + specular(lightDirection));
}