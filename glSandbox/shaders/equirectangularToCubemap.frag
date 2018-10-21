#version 420 core

uniform sampler2D equirectangularMap;

in GS_OUT
{
	vec3 position;
} fs_in;

out vec4 FragColor;

vec2 sphericalToRectangular(vec3 coords)
{
	vec2 uv = vec2(atan(coords.z, coords.x), asin(coords.y));
	const vec2 invAtan = vec2(0.1591, 0.3183);
	uv *= invAtan;
	uv += 0.5;
	return uv;
}
void main()
{
	FragColor = texture(equirectangularMap, sphericalToRectangular(normalize(fs_in.position)));
}