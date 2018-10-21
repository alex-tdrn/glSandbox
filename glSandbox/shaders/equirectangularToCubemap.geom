#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;


uniform mat4 views[6];

out GS_OUT
{
	vec3 position;
} gs_out;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;
		for(int i = 0; i < 3; i++)
		{
			gs_out.position = gl_in[i].gl_Position.xyz;
			gl_Position = views[face] * vec4(gs_out.position, 1.0f);
			EmitVertex();

		}
		EndPrimitive();
	}
}