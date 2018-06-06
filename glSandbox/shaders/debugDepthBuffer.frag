#version 420 core
#define MAX_DIR_LIGHTS 256
#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256
uniform bool linearize;
uniform float nearPlane;
uniform float farPlane;
out vec4 FragColor;

void main()
{
	float z = gl_FragCoord.z;
	if(linearize)
	{
		z = z*2.0f - 1.0f;
		z = (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
	}
	FragColor = vec4(vec3(z), 1.0f);
}
