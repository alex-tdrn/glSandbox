#pragma once
#include "Named.h"
#include "Resources.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Scene;

class Renderer
{
private:
	std::shared_ptr<Scene> scene;
	unsigned int multisampledFramebuffer = 0;
	unsigned int multisampledColorbuffer = 0;
	unsigned int multisampledRenderbuffer = 0;
	unsigned int simpleFramebuffer = 0;
	unsigned int simpleColorbuffer = 0;
	unsigned int simpleRenderbuffer = 0;
	bool explicitRendering = false;
	bool needRedraw = true;

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
			int mode = GL_FILL;
			float lineWidth = 1.0f;
			float pointSize = 1.0f;
		}polygon;
		bool depthTesting = true;
		int depthFunction = GL_LESS;
		bool faceCulling = false;
		int faceCullingMode = GL_BACK;
		int faceCullingOrdering = GL_CCW;
	} pipeline;
	struct{
		resources::ShaderType current = resources::ShaderType::blinn_phong;
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
			float ambientStrength = 0.025f;
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
	Renderer(std::shared_ptr<Scene>& scene);
	Renderer(Renderer const&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer const&) = delete;
	Renderer& operator=(Renderer&&) = delete;
	~Renderer();

private:
	void updateFramebuffers();

public:
	void resizeViewport(int width, int height);
	void setScene(std::shared_ptr<Scene>& scene);
	void render();
	unsigned int getOutput();
	void drawUI(bool* open);

};
