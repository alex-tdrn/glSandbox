#version 420 core
const float magnitude = 0.4f;

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT
{
	vec3 normal;
	vec3 clipSpaceNormal;
} gs_in[];

out GS_OUT
{
	vec3 normal;
} gs_out;

void generateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	gs_out.normal = gs_in[index].normal;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].clipSpaceNormal, 0.0f) * magnitude;
	gs_out.normal = gs_in[index].normal;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	generateLine(0);
	generateLine(1);
	generateLine(2);
}