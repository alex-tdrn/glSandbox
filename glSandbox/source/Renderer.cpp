#include "Renderer.h"
#include "Globals.h"
#include "Lights.h"
#include "Prop.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

Renderer::Renderer(Camera* camera)
{
	setCamera(camera);
	glGenTextures(1, &multisampledColorbuffer);
	glGenRenderbuffers(1, &multisampledRenderbuffer);
	glGenTextures(1, &simpleColorbuffer);
	glGenRenderbuffers(1, &simpleRenderbuffer);
	updateFramebuffers();

	glGenFramebuffers(1, &multisampledFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampledRenderbuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &simpleFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, simpleColorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, simpleRenderbuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &multisampledColorbuffer);
	glDeleteTextures(1, &simpleColorbuffer);

	glDeleteRenderbuffers(1, &multisampledRenderbuffer);
	glDeleteRenderbuffers(1, &simpleRenderbuffer);

	glDeleteFramebuffers(1, &multisampledFramebuffer);
	glDeleteFramebuffers(1, &simpleFramebuffer);
}

bool Renderer::skipFrame() const
{
	if(!camera)
		return true;
	if(explicitRendering)
	{
		if(_shouldRender)
		{
			_shouldRender = false;
			shouldRenderSecondary = true;
		}
		else if(shouldRenderSecondary)
		{
			shouldRenderSecondary = false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		_shouldRender = true;
	};
	return false;
}

void Renderer::configureFramebuffers() const
{
	if(pipeline.samples > 0)
	{
		glEnable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	}
	else
	{
		glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);
	}
	glViewport(0, 0, viewport.width, viewport.height);
}

void Renderer::configureDepthTesting() const
{
	if(pipeline.depthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(pipeline.depthFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void Renderer::configureFaceCulling() const
{
	if(pipeline.faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(pipeline.faceCullingMode);
		glFrontFace(pipeline.faceCullingOrdering);
	}
}

void Renderer::configurePolygonMode() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(geometry.lineWidth);
	glPointSize(geometry.pointSize);
}

void Renderer::clearBuffers() const
{
	glm::vec3 background = scene->getBackground();
	glClearColor(background.r, background.g, background.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::renderAuxiliaryGeometry() const
{
	camera->use();
	glDisable(GL_CULL_FACE);
	ShaderManager::unlit()->use();
	for(auto const& _camera : scene->getAll<Camera>())
	{
		if(camera == _camera || !_camera->isEnabled() || !_camera->getVisualizeFrustum())
			continue;
		ShaderManager::unlit()->set("model", glm::inverse(_camera->getProjectionMatrix() * _camera->getViewMatrix()));
		TextureManager::uvChecker3M()->use(1);
		if(geometry.frustum.mode != geometry.lines)
		{
			ShaderManager::unlit()->set("material.hasMap", geometry.frustum.textured);
			ShaderManager::unlit()->set("material.map", 1);
			ShaderManager::unlit()->set("material.r", true);
			ShaderManager::unlit()->set("material.g", true);
			ShaderManager::unlit()->set("material.b", true);
			ShaderManager::unlit()->set("material.a", true);
			ShaderManager::unlit()->set("material.color", glm::vec3{1.0f, 1.0f, 1.0f});
			MeshManager::box()->use();
		}
		if(geometry.frustum.mode != geometry.triangles)
		{
			ShaderManager::unlit()->set("material.hasMap", false);
			ShaderManager::unlit()->set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
			MeshManager::boxWireframe()->use();
		}
	}
	Node* currentNode = scene->getCurrent();
	if(highlighting.boundingBox && currentNode)
	{
		ShaderManager::unlit()->set("model", currentNode->getBounds().getTransformation());
		ShaderManager::unlit()->set("material.hasMap", false);
		ShaderManager::unlit()->set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
		MeshManager::boxWireframe()->use();
	}
	if(geometry.grid.enabled)
	{
		glLineWidth(geometry.grid.lineWidth);
		ShaderManager::unlit()->set("model", glm::scale(glm::mat4{1.0f}, glm::vec3{geometry.grid.scale}));
		ShaderManager::unlit()->set("material.hasMap", false);
		ShaderManager::unlit()->set("material.color", geometry.grid.color);
		geometry.grid.mainGenerator.get()->use();
		glLineWidth(geometry.grid.lineWidth * 4);
		geometry.grid.subGenerator.get()->use();
		configurePolygonMode();
	}
	configureFaceCulling();
}

void Renderer::renderLights() const
{
	auto drawLights = [&](auto lights){
		for(auto const& light : lights)
		{
			if(!light->isEnabled())
				continue;
			glm::vec3 lightColor = light->getColor() * light->getIntensity() / (light->getIntensity() + 1);
			ShaderManager::unlit()->set("material.hasMap", false);
			ShaderManager::unlit()->set("material.color", lightColor);
			ShaderManager::unlit()->set("model", light->getGlobalTransformation() * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
			MeshManager::box()->use();
		}
	};
	drawLights(scene->getAll<PointLight>());
	drawLights(scene->getAll<SpotLight>());
}

void Renderer::updateShadowMaps() const
{
	const int resolution = 1 << shading.lighting.shadows.resolution;
	auto allocateShadowMap = [&]() -> Texture{
		Texture ret{GL_DEPTH_COMPONENT, resolution, resolution,
			GL_DEPTH_COMPONENT, GL_FLOAT};
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, shading.lighting.shadows.depthComparison);
		static float const borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		return std::move(ret);
	};
	auto resetMaps = [&](auto& lights, auto& shadowMaps){
		shadowMaps.clear();
		shadowMaps.reserve(lights.size());
		for(auto& light : lights)
			shadowMaps.emplace_back(allocateShadowMap());

	};
	resetMaps(scene->getAll<DirectionalLight>(), shading.lighting.shadows.shadowMapsD);
	resetMaps(scene->getAll<SpotLight>(), shading.lighting.shadows.shadowMapsS);
	auto& pointLights = scene->getAll<PointLight>();
	auto& shadowMapsP = shading.lighting.shadows.shadowMapsP;
	shadowMapsP.clear();
	shadowMapsP.reserve(pointLights.size());
	for(auto& light : pointLights)
	{
		shadowMapsP.emplace_back(GL_DEPTH_COMPONENT, resolution, resolution,
			GL_DEPTH_COMPONENT, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, shading.lighting.shadows.depthComparison);
	}
}

void Renderer::renderShadowMaps() const
{
	auto lightsD = scene->getAll<DirectionalLight>();
	auto lightsS = scene->getAll<SpotLight>();
	auto lightsP = scene->getAll<PointLight>();
	auto& shadowMapsD = shading.lighting.shadows.shadowMapsD;
	auto& shadowMapsS = shading.lighting.shadows.shadowMapsS;
	auto& shadowMapsP = shading.lighting.shadows.shadowMapsP;
	if(lightsD.size() != shadowMapsD.size() || 
		lightsS.size() != shadowMapsS.size() ||
		lightsP.size() != shadowMapsP.size())
	{
		updateShadowMaps();
	}

	static unsigned int shadowMappingFBO = [](){
		unsigned int fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		return fbo;
	}();
	const int resolution = 1 << shading.lighting.shadows.resolution;
	glViewport(0, 0, resolution, resolution);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMappingFBO);
	if(shading.lighting.shadows.faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(shading.lighting.shadows.faceCullingMode);
		glFrontFace(GL_CCW);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	int enabledDirectionalLights = 0;
	for(int i = 0; i < lightsD.size(); i++)
	{
		if(!lightsD[i]->isEnabled())
			continue;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapsD[i].getID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		float const projSize = shading.lighting.shadows.directionalLightProjectionSize;
		glm::mat4 lightProjection = glm::ortho(-projSize, projSize, -projSize, projSize, 0.01f, 100.0f);
		glm::vec3 lightDirection = lightsD[i]->getDirection();
		glm::vec3 eye = -lightDirection * projSize;
		glm::vec3 center = eye + lightDirection;
		glm::mat4 lightView = glm::lookAt(eye, center, glm::vec3{0.0f, 1.0f, 0.0f});
		glm::mat4 lightSpace = lightProjection * lightView;
		ShaderManager::shadowMappingUnidirectional()->use();
		ShaderManager::shadowMappingUnidirectional()->set("lightSpace", lightSpace);
		renderProps(ShaderManager::shadowMappingUnidirectional());
		shading.current->use();
		shading.current->set("lightSpacesD[" + std::to_string(enabledDirectionalLights) + "]", lightSpace);
		shading.current->set("dirLights[" + std::to_string(enabledDirectionalLights++) + "].shadowMap", 10 + i);
		shadowMapsD[i].use(10 + i);
	}
	int enabledSpotLights = 0;
	for(int i = 0; i < lightsS.size(); i++)
	{
		if(!lightsS[i]->isEnabled())
			continue;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapsS[i].getID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		float const nearPlane = shading.lighting.shadows.spotLightNearPlane;
		float const farPlane = shading.lighting.shadows.spotLightFarPlane;
		glm::mat4 lightProjection = glm::perspective(glm::radians(lightsS[i]->getOuterCutoff() * 2), 1.0f, nearPlane, farPlane);
		glm::vec3 eye = lightsS[i]->getPosition();
		glm::vec3 center = eye + lightsS[i]->getDirection();
		glm::mat4 lightView = glm::lookAt(eye, center, glm::vec3{0.0f, 1.0f, 0.0f});
		glm::mat4 lightSpace = lightProjection * lightView;
		ShaderManager::shadowMappingUnidirectional()->use();
		ShaderManager::shadowMappingUnidirectional()->set("lightSpace", lightSpace);
		renderProps(ShaderManager::shadowMappingUnidirectional());
		shading.current->use();
		shading.current->set("lightSpacesS[" + std::to_string(enabledSpotLights) + "]", lightSpace);
		shading.current->set("spotLights[" + std::to_string(enabledSpotLights++) + "].shadowMap", 
			10 + i + enabledDirectionalLights);
		shadowMapsS[i].use(10 + i + enabledDirectionalLights);
	}

	int enabledPointLights = 0;
	for(int i = 0; i < lightsP.size(); i++)
	{
		if(!lightsP[i]->isEnabled())
			continue;
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapsP[i].getID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		float const nearPlane = shading.lighting.shadows.pointLightNearPlane;
		float const farPlane = shading.lighting.shadows.pointLightFarPlane;
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
		glm::vec3 eye = lightsP[i]->getPosition();
		std::array<glm::mat4, 6> lightSpaceMatrices = {
			lightProjection * glm::lookAt(eye, eye + glm::vec3{+1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			lightProjection * glm::lookAt(eye, eye + glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			lightProjection * glm::lookAt(eye, eye + glm::vec3{0.0f, +1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, +1.0f}),
			lightProjection * glm::lookAt(eye, eye + glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}),
			lightProjection * glm::lookAt(eye, eye + glm::vec3{0.0f, 0.0f, +1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			lightProjection * glm::lookAt(eye, eye + glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f})
		};
		ShaderManager::shadowMappingOmnidirectional()->use();
		ShaderManager::shadowMappingOmnidirectional()->set("lightPos", eye);
		ShaderManager::shadowMappingOmnidirectional()->set("farPlane", farPlane);
		for(int i = 0; i < 6; i++)
			ShaderManager::shadowMappingOmnidirectional()->set(
			"lightSpaces[" + std::to_string(i) + "]", lightSpaceMatrices[i]);
		renderProps(ShaderManager::shadowMappingOmnidirectional());
		shading.current->use();
		shading.current->set("shadowMappingOmniFarPlane", farPlane);
		shading.current->set("pointLights[" + std::to_string(enabledPointLights++) + "].shadowMap",
			10 + i + enabledDirectionalLights + enabledSpotLights);
		shadowMapsP[i].use(10 + i + enabledDirectionalLights + enabledSpotLights);
	}

	glViewport(0, 0, viewport.width, viewport.height);
	configureFramebuffers();
	configureFaceCulling();
}

void Renderer::configureShaders() const
{
	shading.current->use();
	glm::mat4 viewMatrix = camera->getViewMatrix();
	if(ShaderManager::isLightingShader(shading.current))
	{
		shading.current->set("cameraFarPlane", camera->getFarPlane());
		shading.current->set("ambientColor", scene->getBackground());
		shading.current->set("ambientStrength", shading.lighting.ambientStrength);
		auto useLights = [&](auto const& lights, std::string const& prefix1, std::string const& prefix2){
			int enabledLights = 0;
			for(int i = 0; i < lights.size(); i++)
			{
				if(!lights[i]->isEnabled() && !lights[i]->isHighlighted())
					continue;
				std::string prefix = prefix1 + "[" + std::to_string(enabledLights) + "].";
				lights[i]->use(prefix, viewMatrix, *(shading.current), highlighting.enabled && lights[i]->isHighlighted());
				enabledLights++;
			}
			shading.current->set(prefix2, enabledLights);
		};
		useLights(scene->getAll<DirectionalLight>(), "dirLights", "nDirLights");
		useLights(scene->getAll<PointLight>(), "pointLights", "nPointLights");
		useLights(scene->getAll<SpotLight>(), "spotLights", "nSpotLights");

		shading.current->set("shadowMappingEnabled", shading.lighting.shadows.enabled);
		if(shading.lighting.shadows.enabled)
		{
			shading.current->set("shadowMappingBiasMin", shading.lighting.shadows.bias[0]);
			shading.current->set("shadowMappingBiasMax", shading.lighting.shadows.bias[1]);
			shading.current->set("shadowMappingUsePoisson", shading.lighting.shadows.usePoissonSampling);
			if(shading.lighting.shadows.usePoissonSampling)
			{
				shading.current->set("shadowMappingPoissonVariant", shading.lighting.shadows.poissonVariant);
				shading.current->set("shadowMappingSamples", shading.lighting.shadows.poissonSamples);
				shading.current->set("shadowMappingRadius[0]", shading.lighting.shadows.poissonRadius[0]);
				shading.current->set("shadowMappingRadius[1]", shading.lighting.shadows.poissonRadius[1]);
			}
			else
			{
				shading.current->set("shadowMappingSamples", shading.lighting.shadows.pcfSamples);
				shading.current->set("shadowMappingRadius[0]", shading.lighting.shadows.pcfRadius[0]);
				shading.current->set("shadowMappingRadius[1]", shading.lighting.shadows.pcfRadius[1]);
				shading.current->set("shadowMappingEarlyExit", shading.lighting.shadows.pcfEarlyExit);
			}
			renderShadowMaps();
		}
	}
	else if(shading.current == ShaderManager::refraction())
	{
		/*shading.current->set("perChannel", shading.refraction.perChannel);
		shading.current->set("n1", shading.refraction.n1);
		shading.current->set("n1RGB", shading.refraction.n1RGB);
		shading.current->set("n2", shading.refraction.n2);
		shading.current->set("n2RGB", shading.refraction.n2RGB);*/
	}
	else if(shading.current == ShaderManager::reflection())
	{
		/*if(skybox)
		{
			skybox->use();
			shading.current->set("skybox", 0);
			shading.current->set("S->os", S->getPosition());
		}*/

	}
	else if(shading.current == ShaderManager::debugDepthBuffer())
	{
		shading.current->set("linearize", shading.debugging.depthBufferLinear);
		shading.current->set("nearPlane", camera->getNearPlane());
		shading.current->set("farPlane", camera->getFarPlane());
	}
	else if(shading.current == ShaderManager::debugNormals())
	{
		shading.current->set("viewSpace", shading.debugging.normals.viewSpace);
		shading.current->set("faceNormals", shading.debugging.normals.faceNormals);
		shading.current->set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
		if(shading.debugging.normals.showLines)
		{
			ShaderManager::debugNormalsShowLines()->use();
			ShaderManager::debugNormalsShowLines()->set("lineLength", shading.debugging.normals.lineLength);
			ShaderManager::debugNormalsShowLines()->set("color", shading.debugging.normals.lineColor);
			ShaderManager::debugNormalsShowLines()->set("viewSpace", shading.debugging.normals.viewSpace);
			ShaderManager::debugNormalsShowLines()->set("faceNormals", shading.debugging.normals.faceNormals);
			ShaderManager::debugNormalsShowLines()->set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
			for(auto const& prop : scene->getAll<Prop>())
			{
				if(!prop->isEnabled())
					continue;
				ShaderManager::debugNormalsShowLines()->set("model", prop->getGlobalTransformation());
				prop->getMesh().use();
			}
			shading.current->use();
		}
	}
}

void Renderer::renderHighlightedProps() const
{
	if(!highlighting.enabled)
		return;
	if(highlighting.overlay)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
	}
	auto drawProps = [&](){
		for(auto const& prop : scene->getAll<Prop>())
		{
			if(!prop->isHighlighted())
				continue;
			ShaderManager::unlit()->set("model", prop->getGlobalTransformation());
			prop->getMesh().use();
		}
	};
	ShaderManager::unlit()->use();
	if(geometry.prop.mode != geometry.lines)
	{
		ShaderManager::unlit()->set("material.color", highlighting.color);
		drawProps();
	}
	if(geometry.prop.mode != geometry.triangles)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShaderManager::unlit()->set("material.color", glm::vec3{1.0f - highlighting.color});
		drawProps();
		configurePolygonMode();
	}
	if(highlighting.overlay)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}
}

void Renderer::renderProps(Shader* shader) const
{
	shader->use();
	if(geometry.prop.mode != geometry.lines)
	{
		for(auto const& prop : scene->getAll<Prop>())
		{
			if((!highlighting.enabled || !prop->isHighlighted()) && prop->isEnabled())
			{
				shader->set("model", prop->getGlobalTransformation());
				if(shader == ShaderManager::unlit())
				{
					shader->set("material.r", shading.debugging.unlitShowRedChannel);
					shader->set("material.g", shading.debugging.unlitShowGreenChannel);
					shader->set("material.b", shading.debugging.unlitShowBlueChannel);
					shader->set("material.a", shading.debugging.unlitShowAlphaChannel);
				}
				prop->getMaterial()->use(shader, shading.debugging.unlitMap);
				prop->getMesh().use();
			}
		}
	}

	if(geometry.prop.mode != geometry.triangles)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShaderManager::unlit()->use();
		ShaderManager::unlit()->set("material.hasMap", false);
		ShaderManager::unlit()->set("material.color", glm::vec3(0.0f));

		for(auto const& prop : scene->getAll<Prop>())
		{
			if((!highlighting.enabled || !prop->isHighlighted()) && prop->isEnabled())
			{
				ShaderManager::unlit()->set("model", prop->getGlobalTransformation());
				prop->getMesh().use();
			}
		}
		configurePolygonMode();
	}
}

void Renderer::renderSkybox() const
{
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	if(scene->usesSkybox())
	{
		if(geometry.skybox.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		ShaderManager::skybox()->use();
		ShaderManager::skybox()->set("skybox", 0);
		scene->getSkyBox()->use(0);
		MeshManager::box()->use();
	}
	configureFaceCulling();
	configurePolygonMode();
	configureDepthTesting();
}

void Renderer::updateFramebuffers()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, pipeline.samples > 0 && pipeline.samples < 16 ? pipeline.samples : 1, GL_RGB16F, viewport.width, viewport.height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRenderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, pipeline.samples > 0 && pipeline.samples < 16 ? pipeline.samples : 1, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, simpleRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::resizeViewport(int width, int height)
{
	viewport.width = width;
	viewport.height = height;
	updateFramebuffers();
}

void Renderer::setCamera(Camera* camera)
{
	this->camera = camera;
	scene = camera->getScene();
	shading.lighting.shadows.showMap = -1;
	updateShadowMaps();
	shouldRender();
}

Camera* Renderer::getCamera()
{
	return camera;
}

void Renderer::shouldRender()
{
	_shouldRender = true;
}

void Renderer::render()
{
	if(skipFrame())
		return;
	configureFramebuffers();
	configureDepthTesting();
	configureFaceCulling();
	configurePolygonMode();
	clearBuffers();

	renderAuxiliaryGeometry();
	renderLights();

	configureShaders();
	renderHighlightedProps();
	renderProps(shading.current);

	renderSkybox();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Renderer::getOutput()
{
	if(pipeline.samples > 0)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, simpleFramebuffer);
		glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	return simpleColorbuffer;
}

void Renderer::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin(name.get().data(), open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::Image(ImTextureID(getOutput()), ImVec2(512, 512 / viewport.aspect()), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::NewLine();
	ImGui::Columns(2, nullptr, true);
	ImGui::Checkbox("Explicit Rendering", &explicitRendering);
	ImGui::NextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Camera");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if(ImGui::BeginCombo("###Camera", camera ? camera->name.get().data() : "None"))
	{
		int id = 0;
		if(ImGui::Selectable("None"), camera == nullptr)
			setCamera(nullptr);
		if(camera == nullptr)
			ImGui::SetItemDefaultFocus();
		for(auto& s : SceneManager::getAll())
		{
			ImGui::Text(("Scene: " + s->name.get()).data());
			ImGui::Separator();
			for(auto& _camera : s->getAll<Camera>())
			{
				ImGui::PushID(id++);
				bool isSelected = camera == _camera;
				if(ImGui::Selectable(_camera->name.get().data(), &isSelected))
					setCamera(_camera);
				if(isSelected)
					ImGui::SetItemDefaultFocus();
				ImGui::PopID();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Columns(1);
	ImGui::AlignTextToFramePadding();
	ImGui::Checkbox("Highlighting", &highlighting.enabled);
	ImGui::SameLine();
	ImGui::ColorEdit3("###Highlighting", &highlighting.color[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
	ImGui::SameLine();
	ImGui::Checkbox("Overlay", &highlighting.overlay);
	ImGui::SameLine();
	ImGui::Checkbox("Bounding Box", &highlighting.boundingBox);

	ImGui::NewLine();
	if(ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Line Width");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat("###Width", &geometry.lineWidth, 0.1f, 32.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Point Size");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat("###Size", &geometry.pointSize, 0.1f, 256.0f);

		ImGui::Text("Prop Draw Mode");
		ImGui::PushID(0);
		ImGui::RadioButton("Triangles", reinterpret_cast<int*>(&geometry.prop.mode), geometry.triangles);
		ImGui::SameLine();
		ImGui::RadioButton("Lines", reinterpret_cast<int*>(&geometry.prop.mode), geometry.lines);
		ImGui::SameLine();
		ImGui::RadioButton("Both", reinterpret_cast<int*>(&geometry.prop.mode), geometry.both);
		ImGui::PopID();

		ImGui::Text("Frustum Draw Mode");
		ImGui::PushID(1);
		ImGui::RadioButton("Triangles", reinterpret_cast<int*>(&geometry.frustum.mode), geometry.triangles);
		ImGui::SameLine();
		ImGui::RadioButton("Lines", reinterpret_cast<int*>(&geometry.frustum.mode), geometry.lines);
		ImGui::SameLine();
		ImGui::RadioButton("Both", reinterpret_cast<int*>(&geometry.frustum.mode), geometry.both);
		if(geometry.frustum.mode != geometry.lines)
			ImGui::Checkbox("Textured", &geometry.frustum.textured);
		ImGui::PopID();

		ImGui::Checkbox("Grid", &geometry.grid.enabled);
		if(geometry.grid.enabled)
		{
			ImGui::SameLine();
			geometry.grid.mainGenerator.drawUI();
			ImGui::ColorEdit3("###ColorGrid", &geometry.grid.color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
			ImGui::InputFloat("Scale", &geometry.grid.scale, 0.01f);
			if(std::abs(geometry.grid.scale) <= 0.001f)
				geometry.grid.scale = 0.001f;
			ImGui::Text("Line Width");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("###GridLineWidth", &geometry.grid.lineWidth, 0.1f, 32.0f);
		}
		ImGui::Checkbox("Skybox Wireframe", &geometry.skybox.wireframe);

		ImGui::NewLine();
	}

	if(ImGui::CollapsingHeader("Pipeline"))
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Samples");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		static int samples = pipeline.samples;
		ImGui::SliderInt("###Samples", &samples, 0, 8);
		if(ImGui::IsItemDeactivatedAfterChange())
		{
			pipeline.samples = samples;
			updateFramebuffers();
		}
		else if(!ImGui::IsItemActive())
		{
			samples = pipeline.samples;
		}
		ImGui::PopItemWidth();
		ImGui::Columns(2, nullptr, true);
		ImGui::Checkbox("Depth Testing", &pipeline.depthTesting);
		ImGui::NextColumn();
		ImGui::Checkbox("Face Culling", &pipeline.faceCulling);
		ImGui::Separator();
		ImGui::NextColumn();
		ImGui::Text("Function");
		chooseComparisonFunctionFromCombo(pipeline.depthFunction);
		ImGui::NextColumn();

		ImGui::Text("Mode");
		chooseGLEnumFromCombo(pipeline.faceCullingMode, {
			GL_FRONT_AND_BACK, GL_FRONT, GL_BACK
		});
		ImGui::NewLine();
		ImGui::Text("Ordering");
		chooseGLEnumFromCombo(pipeline.faceCullingOrdering, {
			GL_CCW, GL_CW
		});
		ImGui::Columns(1);

	}
	if(ImGui::CollapsingHeader("Shaders", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto drawShaderOption = [&](Shader* shaderOption){
			if(ImGui::RadioButton(shaderOption->name.get().data(), shading.current == shaderOption))
				shading.current = shaderOption;
		};
		ImGui::Columns(3, nullptr, true);
		ImGui::Text("Lighting");
		drawShaderOption(ShaderManager::pbr());
		drawShaderOption(ShaderManager::blinnPhong());
		drawShaderOption(ShaderManager::phong());
		drawShaderOption(ShaderManager::gouraud());
		drawShaderOption(ShaderManager::flat());
		ImGui::NextColumn();
		ImGui::Text("Debugging");
		drawShaderOption(ShaderManager::unlit());
		drawShaderOption(ShaderManager::debugNormals());
		drawShaderOption(ShaderManager::debugTexCoords());
		drawShaderOption(ShaderManager::debugDepthBuffer());
		ImGui::NextColumn();
		ImGui::Text("Experimental");
		drawShaderOption(ShaderManager::reflection());
		drawShaderOption(ShaderManager::refraction());
		ImGui::Columns(1);
		ImGui::Separator();

		if(ShaderManager::isLightingShader(shading.current))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Ambient Strength");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("###Ambient Strength", &shading.lighting.ambientStrength, 0.0f, 1.0f);
			ImGui::Checkbox("Shadow Mapping", &shading.lighting.shadows.enabled);
			if(shading.lighting.shadows.enabled)
			{
				auto& shadows = shading.lighting.shadows;
				ImGui::Text("Shadow Map Generation ");
				ImGui::Checkbox("Face Culling", &shadows.faceCulling);
				if(shadows.faceCulling)
				{
					ImGui::SameLine();
					chooseGLEnumFromCombo(shadows.faceCullingMode, {
						GL_FRONT_AND_BACK, GL_FRONT, GL_BACK
					});
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Resolution Exponent");
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::InputInt("###Resolution", &shadows.resolution, 1);
				if(shadows.resolution < 1)
					shadows.resolution = 1;
				ImGui::EndGroup();
				if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
					updateShadowMaps();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Directional Light Projection Size");
				ImGui::SameLine();
				ImGui::InputFloat("###directionalLightProjectionSize", &shadows.directionalLightProjectionSize, 0.1f, 1.0f);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Spotlight Near Plane");
				ImGui::SameLine();
				ImGui::InputFloat("###spotlightnearplane", &shadows.spotLightNearPlane, 0.01f, 1.0f);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Spotlight Far Plane");
				ImGui::SameLine();
				ImGui::InputFloat("###spotlightfarplane", &shadows.spotLightFarPlane, 1.0f, 5.0f);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Pointlight Near Plane");
				ImGui::SameLine();
				ImGui::InputFloat("###pointlightnearplane", &shadows.pointLightNearPlane, 0.01f, 1.0f);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Pointlight Far Plane");
				ImGui::SameLine();
				ImGui::InputFloat("###pointlightfarplane", &shadows.pointLightFarPlane, 1.0f, 5.0f);
				std::string currentShadowMapName = "None";
				auto& lightsD = scene->getAll<DirectionalLight>();
				auto& lightsS = scene->getAll<SpotLight>();
				auto& lightsP = scene->getAll<PointLight>();
				int& showMap = shadows.showMap;
				if(showMap > -1)
				{
					if(showMap < lightsD.size())
						currentShadowMapName = lightsD[showMap]->name.get();
					else if(showMap < lightsD.size() + lightsS.size())
						currentShadowMapName = lightsS[showMap - lightsD.size()]->name.get();
					else
						currentShadowMapName = lightsP[showMap - lightsD.size() - lightsS.size()]->name.get();
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("View Shadow Map");
				ImGui::SameLine();
				if(ImGui::BeginCombo("###ViewShadowMap", currentShadowMapName.data()))
				{
					bool isSelected = showMap == -1;
					if(ImGui::Selectable("None", &isSelected))
						showMap = -1;
					if(isSelected)
						ImGui::SetItemDefaultFocus();
					ImGui::Separator();
					int id = 0;
					for(int i = 0; i < lightsD.size(); i++)
					{
						ImGui::PushID(id++);
						isSelected = showMap == i;
						if(ImGui::Selectable(lightsD[i]->name.get().data(), &isSelected))
							showMap = i;
						if(isSelected)
							ImGui::SetItemDefaultFocus();
						ImGui::PopID();
					}
					ImGui::Separator();
					int offset = lightsD.size();
					for(int i = 0; i < lightsS.size(); i++)
					{
						ImGui::PushID(id++);
						isSelected = showMap == offset + i;
						if(ImGui::Selectable(lightsS[i]->name.get().data(), &isSelected))
							showMap = offset + i;
						if(isSelected)
							ImGui::SetItemDefaultFocus();
						ImGui::PopID();
					}
					ImGui::Separator();
					offset += lightsS.size();
					for(int i = 0; i < lightsP.size(); i++)
					{
						ImGui::PushID(id++);
						isSelected = showMap == offset + i;
						if(ImGui::Selectable(lightsP[i]->name.get().data(), &isSelected))
							showMap = offset + i;
						if(isSelected)
							ImGui::SetItemDefaultFocus();
						ImGui::PopID();
					}
					ImGui::EndCombo();
				}
				if(showMap > -1)
				{
					if(showMap < lightsD.size())
						shadows.shadowMapsD[showMap].drawUI();
					else if(showMap < lightsD.size() + lightsS.size())
						shadows.shadowMapsS[showMap - lightsD.size()].drawUI();
					else
						shadows.shadowMapsP[showMap - lightsD.size() - lightsS.size()].drawUI();
				}
				ImGui::Separator();
				ImGui::Text("Shadow Map Sampling");
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Shadow Comparison Function");
				ImGui::SameLine();
				chooseComparisonFunctionFromCombo(shadows.depthComparison);
				
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Min Bias");
				ImGui::SameLine();
				ImGui::InputFloat("###BiasMin", &shadows.bias[0], 0.0001f, 0.0005f, "%.4f");
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Max Bias");
				ImGui::SameLine();
				ImGui::InputFloat("###BiasMax", &shadows.bias[1], 0.0001f, 0.0005f, "%.4f");
				ImGui::Text("Sampling Method ");
				ImGui::SameLine();
				if(ImGui::RadioButton("PCF", !shadows.usePoissonSampling))
					shadows.usePoissonSampling = false;
				ImGui::SameLine();
				if(ImGui::RadioButton("Poisson Disk", shadows.usePoissonSampling))
					shadows.usePoissonSampling = true;
				if(shadows.usePoissonSampling)
				{
					ImGui::Text("Poisson Variant ");
					ImGui::SameLine();
					if(ImGui::RadioButton("Simple", shadows.poissonVariant == shadows.simple))
						shadows.poissonVariant = shadows.simple;
					ImGui::SameLine();
					if(ImGui::RadioButton("Stratified", shadows.poissonVariant == shadows.stratified))
						shadows.poissonVariant = shadows.stratified;
					ImGui::SameLine();
					if(ImGui::RadioButton("Rotated", shadows.poissonVariant == shadows.rotated))
						shadows.poissonVariant = shadows.rotated;
					ImGui::Text("Samples");
					ImGui::SameLine();
					ImGui::SliderInt("###PoissonSamples", &shadows.poissonSamples, 1, 64);
					ImGui::Text("Min Radius");
					ImGui::SameLine();
					ImGui::InputFloat("###PoissonMinRadius", &shadows.poissonRadius[0], 0.1f, 1.0f);
					ImGui::Text("Max Radius");
					ImGui::SameLine();
					ImGui::InputFloat("###PoissonMaxRadius", &shadows.poissonRadius[1], 0.1f, 1.0f);
				}
				else
				{
					ImGui::AlignTextToFramePadding();
					int totalPCFSamples = shadows.pcfSamples * 2 + 1;
					totalPCFSamples *= totalPCFSamples;
					ImGui::Text("Samples(%i)", totalPCFSamples);
					ImGui::SameLine();
					ImGui::InputInt("###PCFSamples", &shadows.pcfSamples, 1, 1);
					ImGui::Text("Min Radius");
					ImGui::SameLine();
					ImGui::InputFloat("###PCFMinRadius", &shadows.pcfRadius[0], 0.1f, 1.0f);
					ImGui::Text("Max Radius");
					ImGui::SameLine();
					ImGui::InputFloat("###PCFMaxRadius", &shadows.pcfRadius[1], 0.1f, 1.0f);
					ImGui::Checkbox("Early Exit", &shadows.pcfEarlyExit);
					if(shadows.pcfSamples < 0)
						shadows.pcfSamples = 0;
				}
			}
		}
		else if(shading.current == ShaderManager::unlit())
		{
			ImGui::Text("Show Map");
			auto drawMapOption = [&](std::string_view title, Material::Map value, bool r, bool g, bool b, bool a){
				if(ImGui::RadioButton(title.data(), shading.debugging.unlitMap == value))
				{
					shading.debugging.unlitMap = value;
					shading.debugging.unlitShowRedChannel = r;
					shading.debugging.unlitShowGreenChannel = g;
					shading.debugging.unlitShowBlueChannel = b;
					shading.debugging.unlitShowAlphaChannel = a;
				}
			};
			ImGui::Columns(2, nullptr, true);
			drawMapOption("None", Material::Map::none, false, false, false, false);
			drawMapOption("Normal", Material::Map::normal, true, true, true, false);
			drawMapOption("Occlusion", Material::Map::occlusion, true, false, false, false);
			ImGui::NextColumn();
			drawMapOption("Emissive", Material::Map::emissive, true, true, true, false);
			drawMapOption("Base Color", Material::Map::baseColor, true, true, true, true);
			drawMapOption("Metallic-Roughness", Material::Map::metallicRoughness, false, true, true, false);
			ImGui::Columns(1);
			ImGui::Checkbox("R", &shading.debugging.unlitShowRedChannel);
			ImGui::SameLine();
			ImGui::Checkbox("G", &shading.debugging.unlitShowGreenChannel);
			ImGui::SameLine();
			ImGui::Checkbox("B", &shading.debugging.unlitShowBlueChannel);
			ImGui::SameLine();
			ImGui::Checkbox("A", &shading.debugging.unlitShowAlphaChannel);
		}
		else if(shading.current == ShaderManager::debugNormals())
		{
			ImGui::Checkbox("View Space", &shading.debugging.normals.viewSpace);
			ImGui::SameLine();
			ImGui::Checkbox("Show Lines", &shading.debugging.normals.showLines);
			ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &shading.debugging.normals.faceNormals);
			ImGui::DragFloat("Explode Magnitude", &shading.debugging.normals.explodeMagnitude, 0.01f);
			if(shading.debugging.normals.showLines)
			{
				ImGui::DragFloat("Line length", &shading.debugging.normals.lineLength, 0.001f);
				ImGui::ColorEdit3("Line color", &shading.debugging.normals.lineColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
			}
		}
		else if(shading.current == ShaderManager::debugDepthBuffer())
		{
			ImGui::Checkbox("Linearize", &shading.debugging.depthBufferLinear);
		}
		else if(shading.current == ShaderManager::refraction())
		{
			ImGui::Checkbox("Per Channel", &shading.refraction.perChannel);
			if(!shading.refraction.perChannel)
			{
				ImGui::DragFloat("First Medium", &shading.refraction.n1, 0.001f);
				ImGui::DragFloat("Second Medium", &shading.refraction.n2, 0.001f);
				shading.refraction.n1RGB = glm::vec3(shading.refraction.n1);
				shading.refraction.n2RGB = glm::vec3(shading.refraction.n2);
			}
			else
			{
				ImGui::DragFloat3("First Medium", &shading.refraction.n1RGB.x, 0.001f);
				ImGui::DragFloat3("Second Medium", &shading.refraction.n2RGB.x, 0.001f);
			}
		}
	}
	if(ImGui::IsAnyItemActive() && ImGui::IsMouseHoveringWindow())
		_shouldRender = true;
	ImGui::Text("Status: ");
	ImGui::SameLine();
	if(_shouldRender)
		ImGui::Text("rendering...");
	else
		ImGui::Text("idle...");
	ImGui::End();
}
