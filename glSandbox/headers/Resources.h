#pragma once

#include "Shader.h"
#include "Model.h"
#include "Scene.h"
#include "Cubemap.h"

#include <glad/glad.h>
#include <imgui.h>

namespace resources
{
	inline unsigned int boxVAO;
	inline unsigned int quadVAO;
	namespace scenes
	{
		inline Scene empty{"Empty"};
		inline Scene simple{"Simple"};
		inline Scene medium{"Medium"};
		inline Scene stressTest{"Stress Test"};
		namespace type
		{
			enum type
			{
				empty,
				simple,
				medium,
				stressTest
			};
		}
		inline int active = type::simple;

		inline Scene& getActiveScene();
	}
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

	for(Scene* scene : {&scenes::empty, &scenes::simple, &scenes::medium, &scenes::stressTest})
	{
		scene->getCamera().setPosition(Position{{1.0f, 0.2f, 1.0f}});
		scene->getCamera().setOrientation(Orientation{215.0f, -5.0f, 0.0f});
	}

	{
		DirectionalLight light;
		light.setColor({1.0f, 1.0f, 1.0f});
		light.setOrientation({250.0f, -60.0f, 0.0f});
		light.setIntensity(0.5f);
		scenes::simple.add(light);

		SpotLight spotlight;
		spotlight.setColor({1.0f, 1.0f, 1.0f});
		spotlight.setPosition({1.0f, 0.0f, 3.0f});
		spotlight.setOrientation({230.0f, 4.0f, 0.0f});
		spotlight.setIntensity(125.0f);
		spotlight.setCutoff(15.0f, 55.0f);
		spotlight.disable();
		scenes::simple.add(spotlight);

		Actor actor(&resources::models::bunny);
		actor.setPosition({0.0f, 0.0f, 0.0f});
		actor.setScale({0.010f, 0.010f, 0.010f});
		scenes::simple.add(actor);

		scenes::simple.getCamera().setPosition(Position{{3.0f, 3.5f, 7.5f}});
		scenes::simple.getCamera().setOrientation(Orientation{235.0f, -25.0f, 0.0f});
	}

	{
		PointLight light;
		light.setColor({1.0f, 200.0f / 255.0f, 0.0f});
		light.setIntensity(5.0f);
		light.setPosition({-1.0f, 0.0f, 2.0f});
		scenes::medium.add(light);
	}
	{
		SpotLight light;
		light.setPosition({-10.0f, 0.0f, -1.5f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, -10.0f, 0.0f});
		light.setColor({0.0f, 1.0f, 0.0f});
		scenes::medium.add(light);

		light.setPosition({-10.0f, 0.0f, 1.0f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, -10.0f, 0.0f});
		light.setColor({1.0f, 0.0f, 0.0f});
		scenes::medium.add(light);

		light.setPosition({-10.0f, 2.0f, -0.25f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, 0.0f, 0.0f});
		light.setColor({0.0f, 0.0f, 1.0f});
		scenes::medium.add(light);

		light.setPosition({-3.0f, 1.5f, 1.0f});
		light.setIntensity(400.0f);
		light.setOrientation({260.0f, -30.0f, 0.0f});
		light.setColor({1.0f, 1.0f, 1.0f});
		scenes::medium.add(light);
	}
	{
		Actor actor(&resources::models::sponza);
		actor.setPosition({0.0f, -1.8f, 0.0f});
		actor.setScale({0.0125f, 0.0125f, 0.0125f});
		scenes::medium.add(actor);
	}
	{
		int idx = 1;
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				Actor actor(&resources::models::bunny);
				actor.setPosition({0.0f - i * 4.0f, -0.75f, 0.0f - j * 2.0f});
				actor.setScale({0.005f, 0.005f, 0.005f});
				scenes::medium.add(actor);
				idx++;
			}
		}
	}

	{
		scenes::stressTest.getCamera().setPosition(Position{{3.0f, 2.0f, 1.0f}});
		scenes::stressTest.getCamera().setOrientation(Orientation{215.0f, -20.0f, 0.0f});
		DirectionalLight light;
		light.setOrientation({225.0f, +60.0f, 0.0f});

		scenes::stressTest.add(light);

		int idx = 1;
		const int n = 10;
		const float stride = 1.0f;
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				Actor actor(&resources::models::nanosuit);
				actor.setPosition({0.0f - i * stride, -1.75f, 0.0f - j * stride});
				actor.setScale({0.15f, 0.15f, 0.15f});
				scenes::stressTest.add(actor);
				idx++;
			}
		}
	}
}

Scene& resources::scenes::getActiveScene()
{
	Scene& ret = [&]() -> Scene&{
		switch(active)
		{
			case type::simple:
				return simple;
			case type::medium:
				return medium;
			case type::stressTest:
				return stressTest;
			default:
				return empty;
		}
	}();
	return ret;
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
		std::string_view comboPreview = scenes::empty.getName();
		switch(scenes::active)
		{
			case scenes::type::simple:
				comboPreview = scenes::simple.getName();
				break;
			case scenes::type::medium:
				comboPreview = scenes::medium.getName();
				break;
			case scenes::type::stressTest:
				comboPreview = scenes::stressTest.getName();
				break;
		}
		if(ImGui::BeginCombo("Active", comboPreview.data()))
		{
			if(ImGui::Selectable(scenes::empty.getName().data(), scenes::active == scenes::type::empty))
			{
				scenes::active = scenes::type::empty;
				valueChanged = true;
			}
			if(ImGui::Selectable(scenes::simple.getName().data(), scenes::active == scenes::type::simple))
			{
				scenes::active = scenes::type::simple;
				valueChanged = true;
			}
			if(ImGui::Selectable(scenes::medium.getName().data(), scenes::active == scenes::type::medium))
			{
				scenes::active = scenes::type::medium;
				valueChanged = true;
			}
			if(ImGui::Selectable(scenes::stressTest.getName().data(), scenes::active == scenes::type::stressTest))
			{
				scenes::active = scenes::type::stressTest;
				valueChanged = true;
			}
			ImGui::EndCombo();
		}
		scenes::getActiveScene().drawUI(false);
		scenes::empty.drawUI();
		scenes::simple.drawUI();
		scenes::medium.drawUI();
		scenes::stressTest.drawUI();
	}
	if(ImGui::CollapsingHeader("Models"))
	{
		valueChanged |= models::bunny.drawUI();
		valueChanged |= models::nanosuit.drawUI();
		valueChanged |= models::sponza.drawUI();
	}
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
	if(valueChanged)
		scenes::getActiveScene().update();

	ImGui::End();
}
