#version 420 core
uniform sampler2D screenTexture;
uniform float gamma;
uniform int tonemapping;
uniform float exposure;
in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;
vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
void main()
{
	vec3 color = texture(screenTexture, fs_in.textureCoordinates).rgb;
	float finalExposure = pow(2, exposure);
	switch(tonemapping)
	{
		//Reinhard
		case 1:
			color = vec3(1.0f) - exp(-color * finalExposure);
		break;
		//Uncharted 2
		case 2:
			color = Uncharted2Tonemap(color * finalExposure);
			vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
			color *= whiteScale;
		break;
		//Hejl Burgess-Dawson
		case 3:
			vec3 x = max(vec3(0), color * finalExposure - 0.004f);
			FragColor.rgb = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
			return;
		break;
	}
	FragColor.rgb = pow(color , vec3(1.0f / gamma));
}

