#include "Renderer.h"
#include "Lights.h"
#include "Prop.h"

Renderer::Renderer(std::shared_ptr<Scene>& scene)
	:scene(scene)
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

void Renderer::setScene(std::shared_ptr<Scene>& scene)
{
	this->scene = scene;
}

void Renderer::render()
{
	if(explicitRendering)
	{
		if(needToRender)
		{
			needToRender = false;
			needToRenderSecondary = true;
		}
		else if(needToRenderSecondary)
		{
			needToRenderSecondary = false;
		}
		else
		{
			return;
		}
	}
	else
	{
		needToRender = true;
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
	if(pipeline.faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(pipeline.faceCullingMode);
		glFrontFace(pipeline.faceCullingOrdering);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	glPolygonMode(GL_FRONT_AND_BACK, pipeline.polygon.mode);
	switch(pipeline.polygon.mode)
	{
		case GL_FILL:
			break;
		case GL_LINE:
			glLineWidth(pipeline.polygon.lineWidth);
			break;
		case GL_POINT:
			glPointSize(pipeline.polygon.pointSize);
			break;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(scene->getBackground().r, scene->getBackground().g, scene->getBackground().b, 1.0f);

	auto const& props = scene->getActiveProps();
	auto const& camera = scene->getCamera();
	auto const& directionalLights = scene->getDirectionalLights();
	auto const& spotLights = scene->getSpotLights();
	auto const& pointLights = scene->getPointLights();

	camera.use();
	resources::shaders[resources::ShaderType::light].use();
	auto drawLights = [&](auto lights){
		for(auto const& light : lights)
		{
			if(!light.isEnabled())
				continue;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light.getPosition());
			model = glm::scale(model, glm::vec3{0.2f});
			resources::shaders[resources::ShaderType::light].set("model", model);
			resources::shaders[resources::ShaderType::light].set("lightColor", light.getColor() * light.getIntensity());
			resources::box().use();
		}
	};
	drawLights(pointLights);
	drawLights(spotLights);

	Shader& activeShader = resources::shaders[shading.current];
	activeShader.use();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	switch(shading.current)
	{
		case resources::ShaderType::blinn_phong:
		case resources::ShaderType::phong:
		case resources::ShaderType::gouraud:
		case resources::ShaderType::flat:
			activeShader.set("material.overrideDiffuse", shading.lighting.override.diffuse);
			activeShader.set("material.overrideSpecular", shading.lighting.override.specular);
			activeShader.set("material.overrideDiffuseColor", shading.lighting.override.diffuseColor);
			activeShader.set("material.overrideSpecularColor", shading.lighting.override.specularColor);
			activeShader.set("ambientStrength", shading.lighting.ambientStrength);
			activeShader.set("material.shininess", shading.lighting.shininess);
			activeShader.set("ambientColor", scene->getBackground());
			//directional lights
			{
				int enabledLights = 0;
				for(int i = 0; i < directionalLights.size(); i++)
				{
					if(!directionalLights[i].isEnabled())
						continue;
					std::string prefix = "dirLights[" + std::to_string(enabledLights) + "].";
					activeShader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4(directionalLights[i].getDirection(), 0.0f)));
					activeShader.set(prefix + "color", directionalLights[i].getColor());
					activeShader.set(prefix + "intensity", directionalLights[i].getIntensity());
					enabledLights++;
				}
				activeShader.set("nDirLights", enabledLights);
			}
			//point lights
			{
				int enabledLights = 0;
				for(int i = 0; i < pointLights.size(); i++)
				{
					if(!pointLights[i].isEnabled())
						continue;
					std::string prefix = "pointLights[" + std::to_string(enabledLights) + "].";
					activeShader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4(pointLights[i].getPosition(), 1.0f)));
					activeShader.set(prefix + "color", pointLights[i].getColor());
					activeShader.set(prefix + "intensity", pointLights[i].getIntensity());
					enabledLights++;
				}
				activeShader.set("nPointLights", enabledLights);
			}
			//spot lights
			{
				int enabledLights = 0;
				for(int i = 0; i < spotLights.size(); i++)
				{
					if(!spotLights[i].isEnabled())
						continue;
					std::string prefix = "spotLights[" + std::to_string(enabledLights) + "].";
					activeShader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4(spotLights[i].getPosition(), 1.0f)));
					activeShader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4(spotLights[i].getDirection(), 0.0f)));
					activeShader.set(prefix + "color", spotLights[i].getColor());
					activeShader.set(prefix + "intensity", spotLights[i].getIntensity());
					activeShader.set(prefix + "innerCutoff", glm::cos(glm::radians(spotLights[i].getInnerCutoff())));
					activeShader.set(prefix + "outerCutoff", glm::cos(glm::radians(spotLights[i].getOuterCutoff())));
					enabledLights++;
				}
				activeShader.set("nSpotLights", enabledLights);
			}
			break;
		case resources::ShaderType::refraction:
			activeShader.set("perChannel", shading.refraction.perChannel);
			activeShader.set("n1", shading.refraction.n1);
			activeShader.set("n1RGB", shading.refraction.n1RGB);
			activeShader.set("n2", shading.refraction.n2);
			activeShader.set("n2RGB", shading.refraction.n2RGB);
			[[fallthrough]];
		case resources::ShaderType::reflection:
			/*if(skybox)
			{
				skybox->use();
				activeShader.set("skybox", 0);
				activeShader.set("cameraPos", camera.getPosition());
			}*/
			break;
		case resources::ShaderType::debugDepthBuffer:
			activeShader.set("linearize", shading.debugging.depthBufferLinear);
			activeShader.set("nearPlane", camera.getNearPlane());
			activeShader.set("farPlane", camera.getFarPlane());
			break;
		case resources::ShaderType::debugNormals:
			if(shading.debugging.normals.showLines)
			{

				//resources::shaders::debugNormalsShowLines.use();
				//resources::shaders::debugNormalsShowLines.set("model", model);

				//for(auto const& mesh : resources::meshes)
					//mesh.use();
				//activeShader.use();
			}
			activeShader.set("viewSpace", shading.debugging.normals.viewSpace);
			activeShader.set("faceNormals", shading.debugging.normals.faceNormals);
			activeShader.set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
			if(shading.debugging.normals.showLines)
			{
				resources::shaders[resources::ShaderType::debugNormalsShowLines].use();
				resources::shaders[resources::ShaderType::debugNormalsShowLines].set("lineLength", shading.debugging.normals.lineLength);
				resources::shaders[resources::ShaderType::debugNormalsShowLines].set("color", shading.debugging.normals.lineColor);
				resources::shaders[resources::ShaderType::debugNormalsShowLines].set("viewSpace", shading.debugging.normals.viewSpace);
				resources::shaders[resources::ShaderType::debugNormalsShowLines].set("faceNormals", shading.debugging.normals.faceNormals);
				resources::shaders[resources::ShaderType::debugNormalsShowLines].set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
				activeShader.use();
			}
			break;
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glBindVertexArray(resources::boxVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for(auto const& prop : props)
	{
		activeShader.set("model", prop->getTransformation());
		activeShader.set("material.hasDiffuseMap", true);
		activeShader.set("material.diffuseMap", 1);
		activeShader.set("material.hasSpecularMap", false);
		activeShader.set("material.hasOpacityMap", false);
		resources::textures::placeholder.use(1);
		if(prop->getMeshIndex())
			resources::meshes[*prop->getMeshIndex()].use();
	}
	//for(auto& actor : actors)
	//actor.draw(activeShader);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	//resources::shaders::outline.use();
	//for(auto& actor : actors)
	//actor.drawOutline(resources::shaders::outline);
	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	//draw skybox
	/*if(skybox)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		skybox->use();
		resources::shaders::skybox.use();
		resources::shaders::skybox.set("skybox", 0);
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
	ImGui::Checkbox("Explicit Rendering", &explicitRendering);
	ImGui::NewLine();
	if(ImGui::CollapsingHeader("Pipeline", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Samples");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if(ImGui::SliderInt("###Samples", &pipeline.samples, 0, 8))
			updateFramebuffers();
		ImGui::PopItemWidth();
		ImGui::Columns(3, nullptr, true);
		ImGui::Text("Polygon Mode");
		ImGui::NextColumn();
		ImGui::Checkbox("Depth Testing", &pipeline.depthTesting);
		ImGui::NextColumn();
		ImGui::Checkbox("Face Culling", &pipeline.faceCulling);
		ImGui::Separator();
		ImGui::NextColumn();
		ImGui::Text("Mode");
		ImGui::RadioButton("GL_FILL", &pipeline.polygon.mode, GL_FILL);
		ImGui::RadioButton("GL_LINE", &pipeline.polygon.mode, GL_LINE);
		ImGui::RadioButton("GL_POINT", &pipeline.polygon.mode, GL_POINT);
		ImGui::NewLine();
		switch(pipeline.polygon.mode)
		{
			case GL_FILL:
				break;
			case GL_LINE:
				ImGui::Text("Width");
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Width", &pipeline.polygon.lineWidth, 0.1f, 32.0f);
				break;
			case GL_POINT:
				ImGui::Text("Size");
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Size", &pipeline.polygon.pointSize, 0.1f, 256.0f);
				break;
		}
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
		ImGui::RadioButton("Blinn-Phong", reinterpret_cast<int*>(&shading.current), resources::ShaderType::blinn_phong);
		ImGui::RadioButton("Phong", reinterpret_cast<int*>(&shading.current), resources::ShaderType::phong);
		ImGui::RadioButton("Gouraud", reinterpret_cast<int*>(&shading.current), resources::ShaderType::gouraud);
		ImGui::RadioButton("Flat", reinterpret_cast<int*>(&shading.current), resources::ShaderType::flat);
		ImGui::NextColumn();
		ImGui::Text("Debugging");
		ImGui::RadioButton("Normals", reinterpret_cast<int*>(&shading.current), resources::ShaderType::debugNormals);
		ImGui::RadioButton("Texture Coordinates", reinterpret_cast<int*>(&shading.current), resources::ShaderType::debugTexCoords);
		ImGui::RadioButton("Depth Buffer", reinterpret_cast<int*>(&shading.current), resources::ShaderType::debugDepthBuffer);
		ImGui::NextColumn();
		ImGui::Text("Experimental");
		ImGui::RadioButton("Reflection", reinterpret_cast<int*>(&shading.current), resources::ShaderType::reflection);
		ImGui::RadioButton("Refraction", reinterpret_cast<int*>(&shading.current), resources::ShaderType::refraction);
		ImGui::Columns(1);
		ImGui::Separator();

		switch(shading.current)
		{
			case resources::ShaderType::blinn_phong:
			case resources::ShaderType::phong:
			case resources::ShaderType::gouraud:
			case resources::ShaderType::flat:
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
				ImGui::Text("Shininess");
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Shininess", &shading.lighting.shininess, 0, 512);
				ImGui::Text("Ambient Strength");
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("###Ambient Strength", &shading.lighting.ambientStrength, 0.0f, 1.0f);
				break;
			case resources::ShaderType::refraction:
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
			case resources::ShaderType::debugNormals:
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
			case resources::ShaderType::debugTexCoords:

				break;
			case resources::ShaderType::debugDepthBuffer:
				ImGui::Checkbox("Linearize", &shading.debugging.depthBufferLinear);
				break;
		}
	}
	if(ImGui::IsAnyItemActive())
		needToRender = true;
	ImGui::Text("Status: ");
	ImGui::SameLine();
	if(needToRender)
		ImGui::Text("rendering...");
	else
		ImGui::Text("idle...");
	ImGui::End();
}
