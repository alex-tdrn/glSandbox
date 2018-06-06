#version 420 core

uniform sampler2D screenTexture;
uniform float offset;
uniform float kernel[9];
uniform float divisor;
in vec2 texCoord;
out vec4 FragColor;

vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
);

void main()
{
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += kernel[i] * vec3(texture(screenTexture, texCoord + offsets[i])) / divisor;

	FragColor = vec4(col, 1.0f);
}
