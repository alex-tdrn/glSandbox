#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;

class Renderer
{
private:
	int viewportWidth = 1920;
	int viewportHeight = 960;
	float aspectRatio = float(viewportWidth) / viewportHeight;
	unsigned int framebuffer;
	unsigned int colorbuffer;
	unsigned int renderbuffer;
	unsigned int simpleFramebuffer;
	unsigned int simpleColorbuffer;
	bool needRedraw = true;
	enum type
	{
		blinn_phong,
		phong,
		gouraud,
		flat,
		reflection,
		refraction,
		debugNormals,
		debugTexCoords,
		debugDepthBuffer
	};
	int active = type::blinn_phong;

	
	int multisamples = 1;
	
	bool wireframe = false;
	bool depthTesting = true;
	bool faceCulling = false;
	bool explicitRendering = false;
	int depthFunction = GL_LESS;
	int faceCullingMode = GL_BACK;
	int faceCullingOrdering = GL_CCW;
	bool overrideDiffuse = false;
	bool overrideSpecular = false;
	glm::vec3 overrideDiffuseColor{1.0f, 1.0f, 1.0f};
	glm::vec3 overrideSpecularColor{1.0f, 1.0f, 1.0f};
	float ambientStrength = 0.025f;
	float shininess = 8;
	bool refractionPerChannel = false;
	float refractionN1 = 1.0f;
	glm::vec3 refractionN1RGB{refractionN1};
	float refractionN2 = 1.52f;
	glm::vec3 refractionN2RGB{refractionN2};
	bool debugDepthBufferLinear = false;
	bool debugNormalsViewSpace = false;
	bool debugNormalsFaceNormals = false;
	float debugNormalsExplodeMagnitude = 0.0f;
	bool debugNormalsShowLines = false;
	float debugNormalsLineLength = 0.015f;
	glm::vec3 debugNormalsLineColor{1.0f, 1.0f, 1.0f};

private:
	Shader& getActiveShader();
	bool isLightingShaderActive();

public:
	void init();
	void render();
	void resizeFramebuffer();
	unsigned int getOutput();
	void drawUI(bool* open);

};
inline Renderer renderer;