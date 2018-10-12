#version 420 core

struct Material
{
	vec3 color;
	bool hasMap;
	bool linear;
	sampler2D map;
	bool r;
	bool g;
	bool b;
	bool a;
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
	{
		FragColor = texture(material.map, fs_in.textureCoordinates);
		if(!material.r)
			FragColor.r = 0;
		if(!material.g)
			FragColor.g = 0;
		if(!material.b)
			FragColor.b = 0;
		if(!material.a)
			FragColor.a = 0;
		if(material.linear)//cancel out gamma
			FragColor = pow(FragColor, vec4(2.2));
	}
	else
	{
		FragColor = vec4(material.color, 1.0f); 
	}
}