#pragma once

#include "Shader.h"
#include "Model.h"
#include "Scene.h"
#include "Cubemap.h"

#include <glad/glad.h>
#include <imgui.h>
#include <vector>

namespace resources
{
	inline unsigned int boxVAO;
	inline unsigned int quadVAO;
	inline std::vector<Mesh> meshes;
	inline std::vector<Scene> scenes;
	inline int activeScene = -1;
	inline float sceneSize = 2.0f;
	namespace models
	{
		inline Model nanosuit{"models/nanosuit/nanosuit.obj", "Nanosuit"};
		inline Model bunny{"models/bunny/stanford-bunny.obj", "Bunny"};
		inline Model sponza{"models/sponza/sponza.obj", "Sponza"};
	}
	namespace shaders
	{
		//rendering
		inline Shader blinn_phong("shaders/phong.vert", "shaders/blinn-phong.frag");
		inline Shader phong("shaders/phong.vert", "shaders/phong.frag");
		inline Shader gouraud("shaders/gouraud.vert", "shaders/gouraud.frag");
		inline Shader flat("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
		inline Shader reflection("shaders/reflection.vert", "shaders/reflection.frag");
		inline Shader refraction("shaders/refraction.vert", "shaders/refraction.frag");
		inline Shader outline("shaders/outline.vert", "shaders/outline.frag");
		inline Shader debugNormals("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
		inline Shader debugNormalsShowLines("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
		inline Shader debugTexCoords("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
		inline Shader debugDepthBuffer("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
		inline Shader light("shaders/light.vert", "shaders/light.frag");
		inline Shader skybox("shaders/skybox.vert", "shaders/skybox.frag");
		//postprocessing
		inline Shader gammaHDR("shaders/pp.vert", "shaders/ppGammaHDR.frag");
		inline Shader passthrough("shaders/pp.vert", "shaders/ppPassthrough.frag");
		inline Shader grayscale("shaders/pp.vert", "shaders/ppBW.frag");
		inline Shader chromaticAberration("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
		inline Shader invert("shaders/pp.vert", "shaders/ppInvert.frag");
		inline Shader convolution("shaders/pp.vert", "shaders/ppConvolution.frag");

		inline void reload();
	}
	namespace textures
	{
		inline Texture placeholder("textures/placeholder.png", true);
	}
	namespace cubemaps
	{
		inline Cubemap skybox("skybox", {
			"cubemaps/skybox/right.jpg", "cubemaps/skybox/left.jpg",
			"cubemaps/skybox/top.jpg", "cubemaps/skybox/bottom.jpg",
			"cubemaps/skybox/front.jpg", "cubemaps/skybox/back.jpg"
			});
		inline Cubemap mp_blizzard("mp_blizzard", {
			"cubemaps/mp_blizzard/blizzard_rt.tga", "cubemaps/mp_blizzard/blizzard_lf.tga",
			"cubemaps/mp_blizzard/blizzard_up.tga", "cubemaps/mp_blizzard/blizzard_dn.tga",
			"cubemaps/mp_blizzard/blizzard_ft.tga", "cubemaps/mp_blizzard/blizzard_bk.tga"
			});
	}
	inline void init();
	inline void drawUI(bool* open);
}

void resources::init()
{
	//create vertex data and buffer objects
	float boxVertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	unsigned int boxVBO;
	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	float quadVertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};


	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void resources::shaders::reload()
{
	resources::shaders::blinn_phong.reload();
	resources::shaders::phong.reload();
	resources::shaders::gouraud.reload();
	resources::shaders::flat.reload();
	resources::shaders::reflection.reload();
	resources::shaders::refraction.reload();
	resources::shaders::outline.reload();
	resources::shaders::debugNormals.reload();
	resources::shaders::debugNormalsShowLines.reload();
	resources::shaders::debugTexCoords.reload();
	resources::shaders::debugDepthBuffer.reload();
	resources::shaders::light.reload();
	resources::shaders::skybox.reload();

	resources::shaders::gammaHDR.reload();
	resources::shaders::passthrough.reload();
	resources::shaders::grayscale.reload();
	resources::shaders::chromaticAberration.reload();
	resources::shaders::invert.reload();
	resources::shaders::convolution.reload();
}

void resources::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Resources", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	bool valueChanged = false;
	ImGui::Indent();
	if(ImGui::CollapsingHeader("Scenes"))
	{
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.25f);
		ImGui::InputFloat("Scene max size", &resources::sceneSize);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if(ImGui::Button("Rescale"))
			scenes[activeScene].fitToBounds(sceneSize);
		ImGui::InputInt("Active scene", &resources::activeScene, 1, 1);
		resources::activeScene = std::clamp<int>(resources::activeScene, 0, resources::scenes.size());
		for(auto& scene : scenes)
			scene.drawUI();
	}
	if(ImGui::CollapsingHeader("Meshes"))
		for(auto& mesh : meshes)
			mesh.drawUI();
	if(ImGui::CollapsingHeader("Cubemaps"))
	{
		valueChanged |= cubemaps::skybox.drawUI();
		valueChanged |= cubemaps::mp_blizzard.drawUI();
	}
	if(ImGui::Button("Reload Shaders"))
	{
		shaders::reload();
		valueChanged = true;
	}
	ImGui::Unindent();
	//if(valueChanged)
		//scenes::getActiveScene().update();

	ImGui::End();
}
