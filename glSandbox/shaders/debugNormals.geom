#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform bool viewSpace;
uniform bool faceNormals;
uniform float explodeMagnitude;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec3 normal;
} gs_in[];

out GS_OUT
{
	vec3 normal;
} gs_out;

void main()
{
	const vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	const vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	const vec3 triangleNormal = normalize(cross(b, a));

	for(int i = 0; i < 3; i++)
	{
		gl_Position = projection * view * model * (gl_in[i].gl_Position + vec4(triangleNormal * explodeMagnitude, 0.0f));
		if(!faceNormals)
			gs_out.normal = gs_in[i].normal;
		else
		{
			if(viewSpace)
				gs_out.normal = mat3(transpose(inverse(view * model))) * triangleNormal;
			else
				gs_out.normal = mat3(transpose(inverse(model))) * triangleNormal;
		}
		EmitVertex();
	}
	EndPrimitive();
}