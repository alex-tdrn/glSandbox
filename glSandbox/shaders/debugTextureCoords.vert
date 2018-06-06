#version 420 core
layout(std140, binding = 0) uniform CameraMatrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform vec2 uvOffset;
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoords;
out vec2 t;
void main()
{
	t = texCoords + uvOffset;
	while(t.x > 1.0f)
		t.x -= 1.0f;
	while(t.y > 1.0f)
		t.y -= 1.0f;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}