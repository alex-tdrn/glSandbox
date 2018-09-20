#version 420 core

struct Material
{
	vec3 color;
	bool hasMap;
	sampler2D map;
};
uniform Material material;

out vec4 FragColor;

in VS_OUT
{
	vec2 textureCoordinates;
} fs_in;

void main()
{
	if(material.hasMap)
		FragColor = texture(material.map, fs_in.textureCoordinates);
	else
    FragColor = vec4(material.color, 1.0f); 
}