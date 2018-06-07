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
uniform float lineLength;

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT
{
	vec3 normal;
} gs_in[];

vec3 triangleNormal;
vec4 triangleCenter;

vec4 translatePosition(vec4 pos)
{
	return projection * view * model * (pos + vec4(triangleNormal * explodeMagnitude, 0.0f));
}
void generateLine(int index)
{
	vec4 p = gl_in[index].gl_Position;
	vec3 n = gs_in[index].normal;

	if(faceNormals)
	{
		p = triangleCenter;
		n = triangleNormal;
	}
	if(viewSpace)
		n = mat3(transpose(inverse(view * model))) * n;
	else
		n = mat3(transpose(inverse(model))) * n;
	gl_Position = translatePosition(p);
	EmitVertex();
	gl_Position = translatePosition(p + vec4(n * lineLength, 0.0f));
	EmitVertex();
	EndPrimitive();
}

void main()
{
	const vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	const vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	triangleNormal = normalize(cross(b, a)), 0.0f;
	if(faceNormals)
	{
		triangleCenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0f;
		generateLine(0);
	}
	else
	{
		generateLine(0);
		generateLine(1);
		generateLine(2);
	}
}