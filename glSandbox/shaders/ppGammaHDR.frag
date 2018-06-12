#version 420 core
uniform sampler2D screenTexture;
uniform float gamma;
uniform int tonemapping;
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
	}

	FragColor.rgb = pow(color , vec3(1.0f / gamma));
}
