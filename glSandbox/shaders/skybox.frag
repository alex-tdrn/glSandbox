#version 420 core
uniform samplerCube skybox;

in VS_OUT
{
	vec3 textureCoordinates;
} fs_in;

out vec4 FragColor;

void main()
{    
	FragColor = texture(skybox, fs_in.textureCoordinates);
}