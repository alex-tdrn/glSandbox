#include "Renderer.h"
#include "Lights.h"
#include "Prop.h"

Renderer::Renderer(Camera* sourceCamera)
	:sourceCamera(sourceCamera)
{
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

void Renderer::setCamera(Camera* sourceCamera)
{
	this->sourceCamera = sourceCamera;
	shouldRender();
}

Camera* Renderer::getCamera()
{
	return sourceCamera;
}

void Renderer::shouldRender()
{
	_shouldRender = true;
}

void Renderer::render()
{
	if(!sourceCamera)
		return;
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
			return;
		}
	}
	else
	{
		_shouldRender = true;
	}

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
	if(pipeline.depthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(pipeline.depthFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto const& scene = sourceCamera->getScene();
	glClearColor(scene.getBackground().r, scene.getBackground().g, scene.getBackground().b, 1.0f);

	auto const& props = scene.getAll<Prop>();
	auto const& directionalLights = scene.getAll<DirectionalLight>();
	auto const& spotLights = scene.getAll<SpotLight>();
	auto const& pointLights = scene.getAll<PointLight>();
	auto const& sourceCameras = scene.getAll<Camera>();

	sourceCamera->use();
	glDisable(GL_CULL_FACE);
	res::shaders()[res::ShaderType::unlit].use();
	for(auto const& camera: sourceCameras)
	{
		if(sourceCamera == camera || !camera->isEnabled() || !camera->getVisualizeFrustum())
			continue;
		res::shaders()[res::ShaderType::unlit].set("model", glm::inverse(camera->getProjectionMatrix() * camera->getViewMatrix()));
		res::textures::placeholder()->use(1);
		if(geometry.frustum.mode != geometry.lines)
		{
			res::shaders()[res::ShaderType::unlit].set("material.hasMap", geometry.frustum.textured);
			res::shaders()[res::ShaderType::unlit].set("material.map", 1);
			res::shaders()[res::ShaderType::unlit].set("material.color", glm::vec3{1.0f, 1.0f, 1.0f});
			res::meshes::box()->use();
		}
		if(geometry.frustum.mode!= geometry.triangles)
		{
			res::shaders()[res::ShaderType::unlit].set("material.hasMap", false);
			res::shaders()[res::ShaderType::unlit].set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
			res::meshes::boxWireframe()->use();
		}
	}
	if(highlighting.boundingBox && scene.getCurrent())
	{
		res::shaders()[res::ShaderType::unlit].set("model", scene.getCurrent()->getBounds().getTransformation());
		res::shaders()[res::ShaderType::unlit].set("material.hasMap", false);
		res::shaders()[res::ShaderType::unlit].set("material.color", glm::vec3{0.0f, 0.0f, 0.0f});
		res::meshes::boxWireframe()->use();
	}
	if(geometry.grid.enabled)
	{
		glLineWidth(geometry.grid.lineWidth);
		res::shaders()[res::ShaderType::unlit].set("model", glm::scale(glm::mat4{1.0f}, glm::vec3{geometry.grid.scale}));
		res::shaders()[res::ShaderType::unlit].set("material.hasMap", false);
		res::shaders()[res::ShaderType::unlit].set("material.color", geometry.grid.color);
		geometry.grid.mainGenerator.get()->use();
		glLineWidth(geometry.grid.lineWidth * 4);
		geometry.grid.subGenerator.get()->use();
	}
	if(pipeline.faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(pipeline.faceCullingMode);
		glFrontFace(pipeline.faceCullingOrdering);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(geometry.lineWidth);
	glPointSize(geometry.pointSize);

	auto drawLights = [&](auto lights){
		for(auto const& light : lights)
		{
			if(!light->isEnabled())
				continue;
			res::shaders()[res::ShaderType::unlit].set("material.hasMap", false);
			res::shaders()[res::ShaderType::unlit].set("material.color", light->getColor() * light->getIntensity());
			res::shaders()[res::ShaderType::unlit].set("model", light->getGlobalTransformation());
			res::meshes::box()->use();
		}
	};
	drawLights(pointLights);
	drawLights(spotLights);

	Shader& activeShader = res::shaders()[shading.current];
	activeShader.use();
	glm::mat4 viewMatrix = sourceCamera->getViewMatrix();
	switch(shading.current)
	{
		case res::ShaderType::blinn_phong:
		case res::ShaderType::phong:
		case res::ShaderType::gouraud:
		case res::ShaderType::flat:
		{
			activeShader.set("material.overrideDiffuse", shading.lighting.override.diffuse);
			activeShader.set("material.overrideSpecular", shading.lighting.override.specular);
			activeShader.set("material.overrideDiffuseColor", shading.lighting.override.diffuseColor);
			activeShader.set("material.overrideSpecularColor", shading.lighting.override.specularColor);
			activeShader.set("ambientStrength", shading.lighting.ambientStrength);
			activeShader.set("material.shininess", shading.lighting.shininess);
			activeShader.set("ambientColor", scene.getBackground());
			auto useLights = [&viewMatrix, &activeShader](auto const& lights, std::string const& prefix1, std::string const& prefix2){
				int enabledLights = 0;
				for(int i = 0; i < lights.size(); i++)
				{
					if(!lights[i]->isEnabled() && !lights[i]->isHighlighted())
						continue;
					std::string prefix = prefix1 + "[" + std::to_string(enabledLights) + "].";
					lights[i]->use(prefix, viewMatrix, activeShader);
					enabledLights++;
				}
				activeShader.set(prefix2, enabledLights);
			};
			useLights(directionalLights, "dirLights", "nDirLights");
			useLights(pointLights, "pointLights", "nPointLights");
			useLights(spotLights, "spotLights", "nSpotLights");
			break;
		}
		case res::ShaderType::refraction:
			activeShader.set("perChannel", shading.refraction.perChannel);
			activeShader.set("n1", shading.refraction.n1);
			activeShader.set("n1RGB", shading.refraction.n1RGB);
			activeShader.set("n2", shading.refraction.n2);
			activeShader.set("n2RGB", shading.refraction.n2RGB);
			[[fallthrough]];
		case res::ShaderType::reflection:
			/*if(skybox)
			{
				skybox->use();
				activeShader.set("skybox", 0);
				activeShader.set("S->os", S->getPosition());
			}*/
			break;
		case res::ShaderType::debugDepthBuffer:
			activeShader.set("linearize", shading.debugging.depthBufferLinear);
			activeShader.set("nearPlane", sourceCamera->getNearPlane());
			activeShader.set("farPlane", sourceCamera->getFarPlane());
			break;
		case res::ShaderType::debugNormals:
			activeShader.set("viewSpace", shading.debugging.normals.viewSpace);
			activeShader.set("faceNormals", shading.debugging.normals.faceNormals);
			activeShader.set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
			if(shading.debugging.normals.showLines)
			{
				res::shaders()[res::ShaderType::debugNormalsShowLines].use();
				res::shaders()[res::ShaderType::debugNormalsShowLines].set("lineLength", shading.debugging.normals.lineLength);
				res::shaders()[res::ShaderType::debugNormalsShowLines].set("color", shading.debugging.normals.lineColor);
				res::shaders()[res::ShaderType::debugNormalsShowLines].set("viewSpace", shading.debugging.normals.viewSpace);
				res::shaders()[res::ShaderType::debugNormalsShowLines].set("faceNormals", shading.debugging.normals.faceNormals);
				res::shaders()[res::ShaderType::debugNormalsShowLines].set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
				for(auto const& prop : props)
				{
					if(prop->isEnabled())
					{
						res::shaders()[res::ShaderType::debugNormalsShowLines].set("model", prop->getGlobalTransformation());
						prop->getMesh().use();
					}
				}
				activeShader.use();
			}
			break;
	}

	if(highlighting.enabled)
	{
		if(highlighting.overlay)
		{
			glEnable(GL_STENCIL_TEST);
			glStencilMask(0xFF);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
		}
		res::shaders()[res::ShaderType::unlit].use();
		if(geometry.prop.mode != geometry.lines)
		{
			res::shaders()[res::ShaderType::unlit].set("material.color", highlighting.color);
			for(auto const& prop : props)
			{
				if(prop->isHighlighted())
				{
					res::shaders()[res::ShaderType::unlit].set("model", prop->getGlobalTransformation());
					prop->getMesh().use();
				}
			}
		}
		if(geometry.prop.mode != geometry.triangles)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			res::shaders()[res::ShaderType::unlit].set("material.color", glm::vec3{1.0f - highlighting.color});
			for(auto const& prop : props)
			{
				if(prop->isHighlighted())
				{
					res::shaders()[res::ShaderType::unlit].set("model", prop->getGlobalTransformation());
					prop->getMesh().use();
				}
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if(highlighting.overlay)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
		}
	}

	activeShader.use();
	if(shading.current == res::ShaderType::unlit)
	{
		activeShader.set("material.hasMap", shading.lighting.unlit.map.has_value());
		if(shading.lighting.unlit.map)
			activeShader.set("material.map", *shading.lighting.unlit.map);
		activeShader.set("material.color", shading.lighting.unlit.surfaceColor);
	}
	else
	{
		activeShader.set("material.hasDiffuseMap", true);
		activeShader.set("material.diffuseMap", 1);
		activeShader.set("material.hasSpecularMap", false);
		activeShader.set("material.hasOpacityMap", false);
	}
	if(geometry.prop.mode != geometry.lines)
	{
		for(auto const& prop : props)
		{
			if((!highlighting.enabled || !prop->isHighlighted()) && prop->isEnabled())
			{
				activeShader.set("model", prop->getGlobalTransformation());
				if(prop->getTexture())
					prop->getTexture()->use(1);
				else
					res::textures::placeholder()->use(1);
				prop->getMesh().use();
			}
		}
	}

	if(geometry.prop.mode != geometry.triangles)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		res::shaders()[res::ShaderType::unlit].use();
		res::shaders()[res::ShaderType::unlit].set("material.hasMap", false);
		res::shaders()[res::ShaderType::unlit].set("material.color", shading.lighting.unlit.lineColor);

		for(auto const& prop : props)
		{
			if((!highlighting.enabled || !prop->isHighlighted()) && prop->isEnabled())
			{
				res::shaders()[res::ShaderType::unlit].set("model", prop->getGlobalTransformation());
				prop->getMesh().use();
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glDisable(GL_STENCIL_TEST);
	//draw skybox
	/*if(skybox)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		skybox->use();
		resources::shaders()::skybox.use();
		resources::shaders()::skybox.set("skybox", 0);
		glBindVertexArray(resources::boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}*/

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
	if(ImGui::BeginCombo("###Camera", sourceCamera ? sourceCamera->getName().data() : "None"))
	{
		int id = 0;
		if(ImGui::Selectable("None"), sourceCamera == nullptr)
			sourceCamera = nullptr;
		if(sourceCamera == nullptr)
			ImGui::SetItemDefaultFocus();
		for(auto& s : res::scenes::getAll())
		{
			ImGui::Text(("Scene: " + s->name.get()).data());
			ImGui::Separator();
			for(auto& c : s->getAll<Camera>())
			{
				ImGui::PushID(id++);
				bool isSelected = sourceCamera == c;
				if(ImGui::Selectable(c->getName().data(), &isSelected))
					setCamera(c);
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
	ImGui::ColorEdit3("###Highlighting", &highlighting.color[0], ImGuiColorEditFlags_NoInputs);
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
			ImGui::ColorEdit3("###ColorGrid", &geometry.grid.color.x, ImGuiColorEditFlags_NoInputs);
			ImGui::InputFloat("Scale", &geometry.grid.scale, 0.01f);
			if(std::abs(geometry.grid.scale) <= 0.001f)
				geometry.grid.scale = 0.001f;
			ImGui::Text("Line Width");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("###GridLineWidth", &geometry.grid.lineWidth, 0.1f, 32.0f);
		}

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
		ImGui::Columns(3, nullptr, true);
		ImGui::Text("Lighting");
		ImGui::RadioButton("Unlit", reinterpret_cast<int*>(&shading.current), res::ShaderType::unlit);
		ImGui::RadioButton("Blinn-Phong", reinterpret_cast<int*>(&shading.current), res::ShaderType::blinn_phong);
		ImGui::RadioButton("Phong", reinterpret_cast<int*>(&shading.current), res::ShaderType::phong);
		ImGui::RadioButton("Gouraud", reinterpret_cast<int*>(&shading.current), res::ShaderType::gouraud);
		ImGui::RadioButton("Flat", reinterpret_cast<int*>(&shading.current), res::ShaderType::flat);
		ImGui::NextColumn();
		ImGui::Text("Debugging");
		ImGui::RadioButton("Normals", reinterpret_cast<int*>(&shading.current), res::ShaderType::debugNormals);
		ImGui::RadioButton("Texture Coordinates", reinterpret_cast<int*>(&shading.current), res::ShaderType::debugTexCoords);
		ImGui::RadioButton("Depth Buffer", reinterpret_cast<int*>(&shading.current), res::ShaderType::debugDepthBuffer);
		ImGui::NextColumn();
		ImGui::Text("Experimental");
		ImGui::RadioButton("Reflection", reinterpret_cast<int*>(&shading.current), res::ShaderType::reflection);
		ImGui::RadioButton("Refraction", reinterpret_cast<int*>(&shading.current), res::ShaderType::refraction);
		ImGui::Columns(1);
		ImGui::Separator();

		switch(shading.current)
		{
			case res::ShaderType::unlit:
				ImGui::Text("Map:");
				if(ImGui::BeginCombo("###Map", shading.lighting.unlit.map ? Material::mapTypeToString(*shading.lighting.unlit.map).data() : "None"))
				{
					if(ImGui::Selectable("None", !shading.lighting.unlit.map))
						shading.lighting.unlit.map = std::nullopt;
					if(!shading.lighting.unlit.map)
						ImGui::SetItemDefaultFocus();
					for(int i = 0; i < Material::Maps::n; i++)
					{
						bool selected = shading.lighting.unlit.map && shading.lighting.unlit.map == i;
						if(ImGui::Selectable(Material::mapTypeToString(Material::Maps(i)).data(), &selected))
							shading.lighting.unlit.map = Material::Maps(i);
						if(selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if(!shading.lighting.unlit.map)
					ImGui::ColorEdit3("Surface", &shading.lighting.unlit.surfaceColor.x, ImGuiColorEditFlags_NoInputs);
				ImGui::ColorEdit3("Line", &shading.lighting.unlit.lineColor.x, ImGuiColorEditFlags_NoInputs);
				break;
			case res::ShaderType::blinn_phong:
			case res::ShaderType::phong:
			case res::ShaderType::gouraud:
			case res::ShaderType::flat:
				ImGui::Checkbox("Override Diffuse", &shading.lighting.override.diffuse);
				if(shading.lighting.override.diffuse)
				{
					ImGui::SameLine();
					ImGui::ColorEdit3("Diffuse", &shading.lighting.override.diffuseColor.x, ImGuiColorEditFlags_NoInputs);

				}
				ImGui::SameLine();
				ImGui::Checkbox("Override Specular", &shading.lighting.override.specular);
				if(shading.lighting.override.specular)
				{
					ImGui::SameLine();
					ImGui::ColorEdit3("Specular", &shading.lighting.override.specularColor.x, ImGuiColorEditFlags_NoInputs);
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Shininess");
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Shininess", &shading.lighting.shininess, 0, 512);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Ambient Strength");
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Ambient Strength", &shading.lighting.ambientStrength, 0.0f, 1.0f);
				break;
			case res::ShaderType::refraction:
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
				break;
			case res::ShaderType::debugNormals:
				ImGui::Checkbox("View Space", &shading.debugging.normals.viewSpace);
				ImGui::SameLine();
				ImGui::Checkbox("Show Lines", &shading.debugging.normals.showLines);
				ImGui::SameLine();
				ImGui::Checkbox("Face Normals", &shading.debugging.normals.faceNormals);
				ImGui::DragFloat("Explode Magnitude", &shading.debugging.normals.explodeMagnitude, 0.01f);
				if(shading.debugging.normals.showLines)
				{
					ImGui::DragFloat("Line length", &shading.debugging.normals.lineLength, 0.001f);
					ImGui::ColorEdit3("Line color", &shading.debugging.normals.lineColor.x, ImGuiColorEditFlags_NoInputs);
				}
				break;
			case res::ShaderType::debugTexCoords:

				break;
			case res::ShaderType::debugDepthBuffer:
				ImGui::Checkbox("Linearize", &shading.debugging.depthBufferLinear);
				break;
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
