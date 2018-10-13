#include "Renderer.h"
#include "Globals.h"
#include "Lights.h"
#include "Prop.h"

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glm::vec3 background = scene->getBackground();
	glClearColor(background.r, background.g, background.b, 1.0f);
}

void Renderer::renderAuxiliaryGeometry() const
{
	camera->use();
	glDisable(GL_CULL_FACE);
	ResourceManager<Shader>::unlit()->use();
	for(auto const& _camera : scene->getAll<Camera>())
	{
		if(camera == _camera || !_camera->isEnabled() || !_camera->getVisualizeFrustum())
			continue;
		ResourceManager<Shader>::unlit()->set("model", glm::inverse(_camera->getProjectionMatrix() * _camera->getViewMatrix()));
		ResourceManager<Texture>::debug()->use(1);
		if(geometry.frustum.mode != geometry.lines)
		{
			ResourceManager<Shader>::unlit()->set("material.hasMap", geometry.frustum.textured);
			ResourceManager<Shader>::unlit()->set("material.map", 1);
			ResourceManager<Shader>::unlit()->set("material.r", true);
			ResourceManager<Shader>::unlit()->set("material.g", true);
			ResourceManager<Shader>::unlit()->set("material.b", true);
			ResourceManager<Shader>::unlit()->set("material.a", true);
			ResourceManager<Shader>::unlit()->set("material.color", glm::vec3{1.0f, 1.0f, 1.0f});
			ResourceManager<Mesh>::box()->use();
		}
		if(geometry.frustum.mode != geometry.triangles)
		{
			ResourceManager<Shader>::unlit()->set("material.hasMap", false);
			ResourceManager<Shader>::unlit()->set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
			ResourceManager<Mesh>::boxWireframe()->use();
		}
	}
	Node* currentNode = scene->getCurrent();
	if(highlighting.boundingBox && currentNode)
	{
		ResourceManager<Shader>::unlit()->set("model", currentNode->getBounds().getTransformation());
		ResourceManager<Shader>::unlit()->set("material.hasMap", false);
		ResourceManager<Shader>::unlit()->set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
		ResourceManager<Mesh>::boxWireframe()->use();
	}
	if(geometry.grid.enabled)
	{
		glLineWidth(geometry.grid.lineWidth);
		ResourceManager<Shader>::unlit()->set("model", glm::scale(glm::mat4{1.0f}, glm::vec3{geometry.grid.scale}));
		ResourceManager<Shader>::unlit()->set("material.hasMap", false);
		ResourceManager<Shader>::unlit()->set("material.color", geometry.grid.color);
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
			ResourceManager<Shader>::unlit()->set("material.hasMap", false);
			ResourceManager<Shader>::unlit()->set("material.color", lightColor);
			ResourceManager<Shader>::unlit()->set("model", light->getGlobalTransformation() * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
			ResourceManager<Mesh>::box()->use();
		}
	};
	drawLights(scene->getAll<PointLight>());
	drawLights(scene->getAll<SpotLight>());
}

void Renderer::updateShadowMaps() const
{
	const int resolution = 1 << shading.lighting.shadows.resolution;

	auto lightsD = scene->getAll<DirectionalLight>();
	auto& shadowMapsD = shading.lighting.shadows.shadowMapsD;
	shadowMapsD.clear();
	shadowMapsD.reserve(lightsD.size());
	for(auto light : lightsD)
		shadowMapsD.emplace_back(GL_DEPTH_COMPONENT, resolution, resolution,
		GL_DEPTH_COMPONENT, GL_FLOAT);
}

void Renderer::renderShadowMaps() const
{
	auto lightsD = scene->getAll<DirectionalLight>();
	auto& shadowMapsD = shading.lighting.shadows.shadowMapsD;
	if(lightsD.size() != shadowMapsD.size())
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
	
	for(int i = 0; i < lightsD.size(); i++)
	{
		if(!lightsD[i]->isEnabled())
			continue;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapsD[i].getID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		float const projSize = shading.lighting.shadows.directionalLightProjectionSize;
		glm::mat4 lightProjection = glm::ortho(-projSize, projSize, -projSize, projSize, 1.0f, 100.0f);
		glm::vec3 lightDirection = lightsD[i]->getDirection();
		glm::vec3 eye = -lightDirection * projSize;
		glm::vec3 center = eye + lightDirection;
		glm::mat4 lightView = glm::lookAt(eye, center, glm::vec3{0.0f, 1.0f, 0.0f});
		glm::mat4 lightSpace = lightProjection * lightView;
		ResourceManager<Shader>::shadowMapping()->use();
		ResourceManager<Shader>::shadowMapping()->set("lightSpace", lightSpace);
		renderProps(ResourceManager<Shader>::shadowMapping());
		shading.current->use();
		shading.current->set("lightSpacesD[" + std::to_string(i) + "]", lightSpace);
		shading.current->set("dirLights[" + std::to_string(i) + "].shadowMap", 10 + i);
		shadowMapsD[i].use(10 + i);
	}
	glViewport(0, 0, viewport.width, viewport.height);
	configureFramebuffers();
}

void Renderer::configureShaders() const
{
	shading.current->use();
	glm::mat4 viewMatrix = camera->getViewMatrix();
	if(ResourceManager<Shader>::isLightingShader(shading.current))
	{
		shading.current->set("ambientColor", scene->getBackground());
		shading.current->set("ambientStrength", shading.lighting.ambientStrength);
		auto useLights = [&](auto const& lights, std::string const& prefix1, std::string const& prefix2){
			int enabledLights = 0;
			for(int i = 0; i < lights.size(); i++)
			{
				if(!lights[i]->isEnabled() && !lights[i]->isHighlighted())
					continue;
				std::string prefix = prefix1 + "[" + std::to_string(enabledLights) + "].";
				lights[i]->use(prefix, viewMatrix, *(shading.current));
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
			shading.current->set("shadowMappingBias", shading.lighting.shadows.bias);
			renderShadowMaps();
		}
	}
	else if(shading.current == ResourceManager<Shader>::refraction())
	{
		/*shading.current->set("perChannel", shading.refraction.perChannel);
		shading.current->set("n1", shading.refraction.n1);
		shading.current->set("n1RGB", shading.refraction.n1RGB);
		shading.current->set("n2", shading.refraction.n2);
		shading.current->set("n2RGB", shading.refraction.n2RGB);*/
	}
	else if(shading.current == ResourceManager<Shader>::reflection())
	{
		/*if(skybox)
		{
			skybox->use();
			shading.current->set("skybox", 0);
			shading.current->set("S->os", S->getPosition());
		}*/

	}
	else if(shading.current == ResourceManager<Shader>::debugDepthBuffer())
	{
		shading.current->set("linearize", shading.debugging.depthBufferLinear);
		shading.current->set("nearPlane", camera->getNearPlane());
		shading.current->set("farPlane", camera->getFarPlane());
	}
	else if(shading.current == ResourceManager<Shader>::debugNormals())
	{
		shading.current->set("viewSpace", shading.debugging.normals.viewSpace);
		shading.current->set("faceNormals", shading.debugging.normals.faceNormals);
		shading.current->set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
		if(shading.debugging.normals.showLines)
		{
			ResourceManager<Shader>::debugNormalsShowLines()->use();
			ResourceManager<Shader>::debugNormalsShowLines()->set("lineLength", shading.debugging.normals.lineLength);
			ResourceManager<Shader>::debugNormalsShowLines()->set("color", shading.debugging.normals.lineColor);
			ResourceManager<Shader>::debugNormalsShowLines()->set("viewSpace", shading.debugging.normals.viewSpace);
			ResourceManager<Shader>::debugNormalsShowLines()->set("faceNormals", shading.debugging.normals.faceNormals);
			ResourceManager<Shader>::debugNormalsShowLines()->set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
			for(auto const& prop : scene->getAll<Prop>())
			{
				if(!prop->isEnabled())
					continue;
				ResourceManager<Shader>::debugNormalsShowLines()->set("model", prop->getGlobalTransformation());
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
			ResourceManager<Shader>::unlit()->set("model", prop->getGlobalTransformation());
			prop->getMesh().use();
		}
	};
	ResourceManager<Shader>::unlit()->use();
	if(geometry.prop.mode != geometry.lines)
	{
		ResourceManager<Shader>::unlit()->set("material.color", highlighting.color);
		drawProps();
	}
	if(geometry.prop.mode != geometry.triangles)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ResourceManager<Shader>::unlit()->set("material.color", glm::vec3{1.0f - highlighting.color});
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
				if(shader == ResourceManager<Shader>::unlit())
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
		ResourceManager<Shader>::unlit()->use();
		ResourceManager<Shader>::unlit()->set("material.hasMap", false);
		ResourceManager<Shader>::unlit()->set("material.color", glm::vec3(0.0f));

		for(auto const& prop : scene->getAll<Prop>())
		{
			if((!highlighting.enabled || !prop->isHighlighted()) && prop->isEnabled())
			{
				ResourceManager<Shader>::unlit()->set("model", prop->getGlobalTransformation());
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
		ResourceManager<Shader>::skybox()->use();
		ResourceManager<Shader>::skybox()->set("skybox", 0);
		scene->getSkyBox()->use(0);
		ResourceManager<Mesh>::box()->use();
	}
	configureFaceCulling();
	configurePolygonMode();
	configureDepthTesting();
}

void Renderer::updateFramebuffers()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, pipeline.samples > 0 && pipeline.samples < 16? pipeline.samples : 1, GL_RGB16F, viewport.width, viewport.height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRenderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, pipeline.samples > 0 && pipeline.samples < 16 ? pipeline.samples : 1, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, NULL);
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
			camera = nullptr;
		if(camera == nullptr)
			ImGui::SetItemDefaultFocus();
		for(auto& s : ResourceManager<Scene>::getAll())
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
	if(ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
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
		ImGui::RadioButton("GL_ALWAYS", &pipeline.depthFunction, GL_ALWAYS);
		ImGui::RadioButton("GL_NEVER", &pipeline.depthFunction, GL_NEVER);
		ImGui::RadioButton("GL_LESS", &pipeline.depthFunction, GL_LESS);
		ImGui::RadioButton("GL_EQUAL", &pipeline.depthFunction, GL_EQUAL);
		ImGui::RadioButton("GL_LEQUAL", &pipeline.depthFunction, GL_LEQUAL);
		ImGui::RadioButton("GL_GREATER", &pipeline.depthFunction, GL_GREATER);
		ImGui::RadioButton("GL_NOTEQUAL", &pipeline.depthFunction, GL_NOTEQUAL);
		ImGui::RadioButton("GL_GEQUAL", &pipeline.depthFunction, GL_GEQUAL);
		ImGui::NextColumn();

		ImGui::Text("Mode");
		ImGui::RadioButton("GL_FRONT_AND_BACK", &pipeline.faceCullingMode, GL_FRONT_AND_BACK);
		ImGui::RadioButton("GL_FRONT", &pipeline.faceCullingMode, GL_FRONT);
		ImGui::RadioButton("GL_BACK", &pipeline.faceCullingMode, GL_BACK);
		ImGui::NewLine();
		ImGui::Text("Ordering");
		ImGui::RadioButton("GL_CCW", &pipeline.faceCullingOrdering, GL_CCW);
		ImGui::RadioButton("GL_CW ", &pipeline.faceCullingOrdering, GL_CW);
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
		drawShaderOption(ResourceManager<Shader>::pbr());
		drawShaderOption(ResourceManager<Shader>::blinnPhong());
		drawShaderOption(ResourceManager<Shader>::phong());
		drawShaderOption(ResourceManager<Shader>::gouraud());
		drawShaderOption(ResourceManager<Shader>::flat());
		ImGui::NextColumn();
		ImGui::Text("Debugging");
		drawShaderOption(ResourceManager<Shader>::unlit());
		drawShaderOption(ResourceManager<Shader>::debugNormals());
		drawShaderOption(ResourceManager<Shader>::debugTexCoords());
		drawShaderOption(ResourceManager<Shader>::debugDepthBuffer());
		ImGui::NextColumn();
		ImGui::Text("Experimental");
		drawShaderOption(ResourceManager<Shader>::reflection());
		drawShaderOption(ResourceManager<Shader>::refraction());
		ImGui::Columns(1);
		ImGui::Separator();

		if(ResourceManager<Shader>::isLightingShader(shading.current))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Ambient Strength");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("###Ambient Strength", &shading.lighting.ambientStrength, 0.0f, 1.0f);
			ImGui::Checkbox("Shadow Mapping", &shading.lighting.shadows.enabled);
			if(shading.lighting.shadows.enabled)
			{
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text("Resolution: 2^");
				ImGui::SameLine();
				ImGui::InputInt("###Resolution", &shading.lighting.shadows.resolution, 1);
				if(shading.lighting.shadows.resolution < 1)
					shading.lighting.shadows.resolution = 1;
				ImGui::EndGroup();
				if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
					updateShadowMaps();
				ImGui::Text("Bias");
				ImGui::SameLine();
				ImGui::InputFloat("###Bias", &shading.lighting.shadows.bias, 0.001f, 0.005f);
				ImGui::Text("Directional Light Projection Size");
				ImGui::SameLine();
				ImGui::InputFloat("###directionalLightProjectionSize", &shading.lighting.shadows.directionalLightProjectionSize, 0.1f, 1.0f);
				std::string currentShadowMapName = "None";
				auto& lightsD = scene->getAll<DirectionalLight>();
				if(shading.lighting.shadows.showMap > -1)
				{
					currentShadowMapName = lightsD[shading.lighting.shadows.showMap]->name.get();
				}
				if(ImGui::BeginCombo("View Shadow Map", currentShadowMapName.data()))
				{
					bool isSelected = shading.lighting.shadows.showMap == -1;
					if(ImGui::Selectable("None", &isSelected))
						shading.lighting.shadows.showMap = -1;
					if(isSelected)
						ImGui::SetItemDefaultFocus();
					ImGui::Separator();
					int id = 0;
					for(int i = 0; i < lightsD.size(); i++)
					{
						ImGui::PushID(id++);
						isSelected = shading.lighting.shadows.showMap == i;
						if(ImGui::Selectable(lightsD[i]->name.get().data(), &isSelected))
							shading.lighting.shadows.showMap = i;
						if(isSelected)
							ImGui::SetItemDefaultFocus();
						ImGui::PopID();
					}
					ImGui::EndCombo();
				}
				if(shading.lighting.shadows.showMap > -1)
					shading.lighting.shadows.shadowMapsD[shading.lighting.shadows.showMap].drawUI();
			}
		}
		else if(shading.current == ResourceManager<Shader>::unlit())
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
		else if(shading.current == ResourceManager<Shader>::debugNormals())
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
		else if(shading.current == ResourceManager<Shader>::debugDepthBuffer())
		{
			ImGui::Checkbox("Linearize", &shading.debugging.depthBufferLinear);
		}
		else if(shading.current == ResourceManager<Shader>::refraction())
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
