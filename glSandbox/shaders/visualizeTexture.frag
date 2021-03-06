#version 420 core

uniform sampler2D textureLocation;
uniform bool visualizeChannel[4];
uniform bool linear;
uniform bool linearize;
uniform float nearPlane;
uniform float farPlane;

in VS_OUT{
	vec2 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = texture(textureLocation, fs_in.textureCoordinates);
	if(linearize)
	{
		FragColor = FragColor * 2.0f - 1.0f;
		FragColor = (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - FragColor * (farPlane - nearPlane));
		FragColor /= farPlane;
	}
	if(linear)//cancel out gamma
			FragColor = pow(FragColor, vec4(2.2));
	for(int i = 0; i < 4; i++)
		if(!visualizeChannel[i])
			FragColor[i] = 0.0f;

}