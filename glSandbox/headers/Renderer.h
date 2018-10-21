#pragma once
#include "Named.h"
#include "ShaderManager.h"
#include "Material.h"
#include "Grid.h"
#include "Texture.h"
#include "Cubemap.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Camera;
class Scene;

class Renderer
{
private:
	Camera* camera = nullptr;
	Scene* scene = nullptr;
	unsigned int multisampledFramebuffer = 0;
	unsigned int multisampledColorbuffer = 0;
	unsigned int multisampledRenderbuffer = 0;
	unsigned int simpleFramebuffer = 0;
	unsigned int simpleColorbuffer = 0;
	unsigned int simpleRenderbuffer = 0;
	bool explicitRendering = false;
	mutable bool _shouldRender = true;
	mutable bool shouldRenderSecondary = true;
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
		bool depthTesting = true;
		int depthFunction = GL_LESS;
		bool faceCulling = true;
		int faceCullingMode = GL_BACK;
		int faceCullingOrdering = GL_CCW;
	} pipeline;
	struct{
		bool enabled = false;
		glm::vec3 color{1.0f};
		bool overlay = false;
		bool boundingBox = false;
	}highlighting;
	struct{
		Shader* current = ShaderManager::unlit();
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
				bool viewSpace = false;
				bool faceNormals = false;
				float explodeMagnitude = 0.0f;
				bool showLines = false;
				float lineLength = 0.015f;
				glm::vec3 lineColor{1.0f, 1.0f, 1.0f};
			}normals;
			bool depthBufferLinear = false;
			Material::Map unlitMap = Material::Map::baseColor;
			bool unlitShowRedChannel = true;
			bool unlitShowGreenChannel = true;
			bool unlitShowBlueChannel = true;
			bool unlitShowAlphaChannel = true;
		}debugging;
		struct{
			struct
			{
				bool enabled = false;
				bool faceCulling = true;
				int faceCullingMode = GL_BACK;
				int resolution = 11;
				float directionalLightProjectionSize = 10.0f;
				float spotLightNearPlane = 0.1f;
				float spotLightFarPlane = 20.0f;
				float pointLightNearPlane = 0.1f;
				float pointLightFarPlane = 100.0f;
				mutable std::vector<Texture> shadowMapsD;
				mutable std::vector<Texture> shadowMapsS;
				mutable std::vector<Cubemap> shadowMapsP;
				int showMap = -1;
				int depthComparison = GL_LEQUAL;
				float bias[2] = {0.0005f, 0.0020f};
				bool usePoissonSampling = false;
				enum PoissonSampling{
					simple,
					stratified,
					rotated
				}poissonVariant = simple;
				int poissonSamples = 32;
				float poissonRadius[2] = {1.5f, 20.0f};
				int pcfSamples = 2;
				float pcfRadius[2] = {0.5f, 10.0f};
				bool pcfEarlyExit = true;
			}shadows;
			float ambientStrength = 1.0f;
		}lighting;
	}shading;
	struct{
		enum Mode
		{
			triangles,
			lines,
			both
		};
		struct{
			Mode mode = lines;
			bool textured = false;
		}frustum;
		struct{
			Mode mode = triangles;
		}prop;
		struct{
			bool enabled = false;
			Grid mainGenerator{64};
			Grid subGenerator{2};
			float scale = 32.0f;
			float lineWidth = 2.0f;
			glm::vec3 color{0.0f};
		}grid;
		struct{
			bool wireframe = false;
		}skybox;
		float lineWidth = 3.0f;
		float pointSize = 2.0f;
	}geometry;
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
	bool skipFrame() const;
	void configureFramebuffers() const;
	void configureDepthTesting() const;
	void configureFaceCulling() const;
	void configurePolygonMode() const;
	void clearBuffers() const;
	void renderAuxiliaryGeometry() const;
	void renderLights() const;
	void updateShadowMaps() const;
	void renderShadowMaps() const;
	void configureShaders() const;
	void renderHighlightedProps() const;
	void renderProps(Shader* shader) const;
	void renderSkybox() const;
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
