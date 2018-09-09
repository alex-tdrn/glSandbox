#pragma once
#include "Named.h"
#include "Resources.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Camera;

class Renderer
{
private:
	Camera* sourceCamera = nullptr;
	unsigned int multisampledFramebuffer = 0;
	unsigned int multisampledColorbuffer = 0;
	unsigned int multisampledRenderbuffer = 0;
	unsigned int simpleFramebuffer = 0;
	unsigned int simpleColorbuffer = 0;
	unsigned int simpleRenderbuffer = 0;
	bool explicitRendering = true;
	bool _shouldRender = true;
	bool shouldRenderSecondary = true;
	struct {
		int width = 1920;
		int height = 960;
		float aspect()
		{
			return float(width) / height;
		}
	} viewport;
	struct{
		int samples = 0;
		struct{
			enum Mode
			{
				triangles,
				lines,
				both
			};
			Mode propMode = both;
			Mode frustumMode = lines;
			float lineWidth = 3.0f;
			float pointSize = 2.0f;
		}polygon;
		bool depthTesting = true;
		int depthFunction = GL_LESS;
		bool faceCulling = true;
		int faceCullingMode = GL_BACK;
		int faceCullingOrdering = GL_CCW;
	} pipeline;
	struct{
		bool enabled = true;
		glm::vec3 color{1.0f};
		bool overlay = false;
	}highlighting;
	struct{
		res::ShaderType current = res::ShaderType::blinn_phong;
		struct
		{
			bool perChannel = false;
			float n1 = 1.0f;
			glm::vec3 n1RGB{n1};
			float n2 = 1.52f;
			glm::vec3 n2RGB{n2};
		}refraction;
		struct{
			struct{
				bool diffuse = false;
				bool specular = false;
				glm::vec3 diffuseColor{1.0f, 1.0f, 1.0f};
				glm::vec3 specularColor{1.0f, 1.0f, 1.0f};
			}override;
			float ambientStrength = 1.0f;
			float shininess = 128;
		}lighting;
		struct{
			struct{
				bool viewSpace = false;
				bool faceNormals = false;
				float explodeMagnitude = 0.0f;
				bool showLines = false;
				float lineLength = 0.015f;
				glm::vec3 lineColor{1.0f, 1.0f, 1.0f};
			}normals;
			bool depthBufferLinear = false;
		}debugging;
	}shading;

public:
	Name<Renderer> name{"renderer"};

public:
	Renderer(Camera* camera = nullptr);
	Renderer(Renderer const&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer const&) = delete;
	Renderer& operator=(Renderer&&) = delete;
	~Renderer();

private:
	void updateFramebuffers();

public:
	void resizeViewport(int width, int height);
	void setCamera(Camera* camera);
	Camera* getCamera();
	void shouldRender();
	void render();
	unsigned int getOutput();
	void drawUI(bool* open);

};
