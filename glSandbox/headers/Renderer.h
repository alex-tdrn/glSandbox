#pragma once
#include "Named.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;
class Scene;

class Renderer
{
private:
	unsigned int multisampledFramebuffer = 0;
	unsigned int multisampledColorbuffer = 0;
	unsigned int multisampledRenderbuffer = 0;
	unsigned int simpleFramebuffer = 0;
	unsigned int simpleColorbuffer = 0;
	unsigned int simpleRenderbuffer = 0;
	int viewportWidth = 1920;
	int viewportHeight = 960;
	float aspectRatio = float(viewportWidth) / viewportHeight;
	int multisamples = 0;

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

public:
	Name<Renderer> name{"renderer"};

public:
	Renderer();
	Renderer(Renderer const&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer const&) = delete;
	Renderer& operator=(Renderer&&) = delete;
	~Renderer();

private:
	Shader& getActiveShader();
	bool isLightingShaderActive();
	void updateFramebuffers();

public:
	void resizeViewport(int width, int height);
	void render(Scene const& scene);
	unsigned int getOutput();
	void drawUI(bool* open);

};
