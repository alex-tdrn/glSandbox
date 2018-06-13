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

void main()
{
	vec3 color = texture(screenTexture, fs_in.textureCoordinates).rgb;
	switch(tonemapping)
	{
		//Reinhard
		case 1:
			color /= color + vec3(1.0f);
		break;
		//Exposure
		case 2:
			/*float d = 0.001f;
			vec3 avg = textureLod(screenTexture, fs_in.textureCoordinates, 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(0.0f, d), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(0.0f, -d), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(d, d), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(d, -d), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(-d, d), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(-d, -d), 5.0).rgb;

			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(d, 0.0f), 5.0).rgb;
			avg += textureLod(screenTexture, fs_in.textureCoordinates + vec2(-d, 0.0f), 5.0).rgb;
			avg /= 9;
			float ex = 0.5f / (0.3f * avg.r + 0.59f * avg.g + 0.11f * avg.b);*/
			color = vec3(1.0f) - exp(-color * exposure);
		break;
	}

	FragColor.rgb = pow(color , vec3(1.0f / gamma));
}
