#include "Scene.h"
#include "Globals.h"
#include "Util.h"

#include <glad/glad.h>
#include <imgui.h>
Scene::Scene(std::string const name)
	: name(name)
{
	
}

void Scene::init()
{
	initialized = true;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, settings::rendering::multisamples, GL_RGB16F, info::windowWidth, info::windowHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorbuffer, 0);

	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings::rendering::multisamples, GL_DEPTH24_STENCIL8, info::windowWidth, info::windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &simpleFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &simpleColorbuffer);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, info::windowWidth, info::windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, simpleColorbuffer, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string_view const Scene::getName()
{
	return name;
}

void Scene::add(DirectionalLight light)
{
	directionalLights.push_back(std::move(light));
}

void Scene::add(PointLight light)
{
	pointLights.push_back(std::move(light));
}

void Scene::add(SpotLight light)
{
	spotLights.push_back(std::move(light));
}

void Scene::add(Actor actor)
{
	actors.push_back(std::move(actor));
}

void Scene::update()
{
	needRedraw = true;
}

void Scene::updateFramebuffer()
{
	update();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, settings::rendering::multisamples, GL_RGB16F, info::windowWidth, info::windowHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings::rendering::multisamples, GL_DEPTH24_STENCIL8, info::windowWidth, info::windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, info::windowWidth, info::windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::setBackgroundColor(glm::vec3 color)
{
	backgroundColor = color;
}

void Scene::draw()
{
	if(!initialized)
		init();
	using namespace settings::rendering;
	if(explicitRendering && !needRedraw)
		return;
	if(explicitRendering)
		needRedraw = false;
	else
		needRedraw = true;

	ImGui::Text("rendering scene..");
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);

	glEnable(GL_MULTISAMPLE);
	if(depthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(depthFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if(faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(faceCullingMode);
		glFrontFace(faceCullingOrdering);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}


	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	camera.use();
	resources::shaders::light.use();
	glBindVertexArray(resources::boxVAO);
	auto drawLights = [&](auto lights){
		for(auto const& light : lights)
		{
			if(!light.isEnabled())
				continue;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light.getPosition());
			model = glm::scale(model, glm::vec3{0.2f});
			resources::shaders::light.set("model", model);
			resources::shaders::light.set("lightColor", light.getColor());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	};
	drawLights(pointLights);
	drawLights(spotLights);
	Shader& activeShader = getActiveShader();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	if(isLightingShaderActive())
	{
		activeShader.set("ambientColor", backgroundColor);
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
	}
	else if(active == type::reflection || active == type::refraction)
	{
		if(skybox)
		{
			skybox->use();
			activeShader.set("skybox", 0);
			activeShader.set("cameraPos", camera.getPosition());
		}
	}
	else if(active == type::debugDepthBuffer)
	{
		activeShader.set("nearPlane", camera.getNearPlane());
		activeShader.set("farPlane", camera.getFarPlane());
	}
	else if(active == type::debugNormals)
	{
		if(settings::rendering::debugNormalsShowLines)
		{
			resources::shaders::debugNormalsShowLines.use();
			for(auto& actor : actors)
				actor.draw(resources::shaders::debugNormalsShowLines);
			activeShader.use();
		}
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	for(auto& actor : actors)
		actor.draw(activeShader);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	resources::shaders::outline.use();
	for(auto& actor : actors)
		actor.drawOutline(resources::shaders::outline);
	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	//draw skybox
	if(skybox)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		skybox->use();
		resources::shaders::skybox.use();
		resources::shaders::skybox.set("skybox", 0);
		glBindVertexArray(resources::boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

unsigned int Scene::getColorbuffer() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, simpleFramebuffer);
	glBlitFramebuffer(0, 0, info::windowWidth, info::windowHeight, 0, 0, info::windowWidth, info::windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	return simpleColorbuffer;
}

Camera& Scene::getCamera()
{
	return camera;
}

void Scene::drawUI(bool const showHeader)
{
	IDGuard idGuard{this};

	if(!showHeader || ImGui::TreeNode(name.data()))
	{
		ImGui::Indent();
		if(ImGui::ColorEdit3("Background", &backgroundColor.x, ImGuiColorEditFlags_NoInputs))
			update();
		std::string_view comboPreview = "None";
		if(skybox)
			comboPreview = skybox->getName();
		if(ImGui::BeginCombo("Skybox", comboPreview.data()))
		{
			if(ImGui::Selectable("None", !skybox))
			{
				skybox = nullptr;
				update();
			}
			if(ImGui::Selectable(resources::cubemaps::skybox.getName().data(), skybox == &resources::cubemaps::skybox))
			{
				skybox = &resources::cubemaps::skybox;
				update();
			}
			if(ImGui::Selectable(resources::cubemaps::mp_blizzard.getName().data(), skybox == &resources::cubemaps::mp_blizzard))
			{
				skybox = &resources::cubemaps::mp_blizzard;
				update();
			}
			ImGui::EndCombo();
		}
		if(camera.drawUI())
			update();
		auto generateListUI = [&](auto& data, std::optional<std::string_view const> const name = std::nullopt){
		if(!name.has_value() || ImGui::TreeNode((*name).data()))
		{
			if(ImGui::Button("Disable All"))
			{
				update();
				for(auto& entity : data)
					entity.disable();
			}
			ImGui::SameLine();
			if(ImGui::Button("Enable All"))
			{
				update();
				for(auto& entity : data)
					entity.enable();
			}
			if(ImGui::Button("Add New"))
			{
				update();
				data.emplace_back();
			}
			int removeIdx = -1;
			for(int i = 0; i < data.size(); i++)
			{
				if(ImGui::TreeNode(std::to_string(i).data()))
				{
					if(ImGui::Button("Remove"))
						removeIdx = i;
					if(data[i].drawUI())
						update();
					ImGui::TreePop();
				}
			}
			if(removeIdx != -1)
			{
				data.erase(data.begin() + removeIdx);
				update();
			}
			if(name.has_value())
				ImGui::TreePop();
			}
		};
		if(ImGui::CollapsingHeader("Actors"))
		{
			generateListUI(actors);
		}
		if(ImGui::CollapsingHeader("Lights"))
		{
			if(ImGui::Button("Disable All"))
			{
				update();
				for(auto& light : directionalLights)
					light.disable();
				for(auto& light : pointLights)
					light.disable();
				for(auto& light : spotLights)
					light.disable();
			}
			ImGui::SameLine();
			if(ImGui::Button("Enable All"))
			{
				update();
				for(auto& light : directionalLights)
					light.enable();
				for(auto& light : pointLights)
					light.enable();
				for(auto& light : spotLights)
					light.enable();
			}
			generateListUI(directionalLights, "Directional Lights");
			generateListUI(pointLights, "Point Lights");
			generateListUI(spotLights, "Spot Lights");
		}
		ImGui::Unindent();
		if(showHeader)
			ImGui::TreePop();
	}
}