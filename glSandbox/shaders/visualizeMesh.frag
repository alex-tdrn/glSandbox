#version 420 core
uniform int visualizeAttribute;
uniform vec3 color;
out vec3 FragColor;

in VS_OUT{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 textureCoordinates;
} fs_in;


void main()
{
	switch(visualizeAttribute)
	{
		case -1:
			FragColor = color;
			return;
		case 0:
			FragColor = fs_in.position;	
			return;
		case 1:
			FragColor = fs_in.normal;	
			return;
		case 2:
			FragColor = fs_in.tangent;	
			return;
		case 3:
			FragColor = vec3(fs_in.textureCoordinates, 0.0f);	
			return;
	}
}