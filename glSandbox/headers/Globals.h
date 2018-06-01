#pragma once
#include "Shader.h"
#include "Model.h"
#include "Scene.h"
#include "PostProcessingStep.h"
#include "Cubemap.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <vector>

namespace resources
{
	inline Scene scene;
	inline unsigned int boxVAO;
	inline unsigned int quadVAO;
	
	namespace models
	{
		inline Model nanosuit{"models/nanosuit/nanosuit.obj", "Nanosuit"};
		inline Model sponza{"models/sponza/sponza.obj", "Sponza"};
	}
	namespace shaders
	{
		//rendering
		inline Shader phong("shaders/phong.vert", "shaders/phong.frag");
		inline Shader gouraud("shaders/gouraud.vert", "shaders/gouraud.frag");
		inline Shader outline("shaders/outline.vert", "shaders/outline.frag");
		inline Shader debugNormals("shaders/debugNormals.vert", "shaders/debugNormals.frag");
		inline Shader debugTexCoords("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
		inline Shader debugDepthBuffer("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
		inline Shader light("shaders/light.vert", "shaders/light.frag");
		inline Shader skybox("shaders/skybox.vert", "shaders/skybox.frag");
		//postprocessing
		inline Shader passthrough("shaders/ppPassthrough.vert", "shaders/ppPassthrough.frag");
		inline Shader grayscale("shaders/ppBW.vert", "shaders/ppBW.frag");
		inline Shader invert("shaders/ppInvert.vert", "shaders/ppInvert.frag");
		inline Shader convolution("shaders/ppConvolution.vert", "shaders/ppConvolution.frag");

		inline void reload();
	}
	namespace cubemaps
	{
		inline Cubemap skybox("skybox",{
			"cubemaps/skybox/right.jpg", "cubemaps/skybox/left.jpg",
			"cubemaps/skybox/top.jpg", "cubemaps/skybox/bottom.jpg",
			"cubemaps/skybox/front.jpg", "cubemaps/skybox/back.jpg"
			});
		inline Cubemap mp_blizzard("mp_blizzard",{
			"cubemaps/mp_blizzard/blizzard_rt.tga", "cubemaps/mp_blizzard/blizzard_lf.tga",
			"cubemaps/mp_blizzard/blizzard_up.tga", "cubemaps/mp_blizzard/blizzard_dn.tga",
			"cubemaps/mp_blizzard/blizzard_ft.tga", "cubemaps/mp_blizzard/blizzard_bk.tga"
			});
	}
	inline void init();
	inline void drawUI();
}

namespace info
{
	inline int windowWidth = 1920;
	inline int windowHeight = 960;
	inline void drawUI();
}

namespace settings
{

	namespace rendering
	{
		enum type
		{
			phong,
			gouraud,
			debugNormals,
			debugTexCoords,
			debugDepthBuffer
		};
		inline int active = type::phong;	

		inline bool vsync = true;
		inline bool wireframe = false;
		inline bool depthTesting = true;
		inline bool faceCulling = false;
		inline bool explicitRendering = true;
		inline int depthFunction = GL_LESS;
		inline int faceCullingMode = GL_BACK;
		inline int faceCullingOrdering = GL_CCW;
		inline bool overrideDiffuse = false;
		inline bool overrideSpecular = false;
		inline glm::vec3 overrideDiffuseColor{1.0f, 1.0f, 1.0f};
		inline glm::vec3 overrideSpecularColor{1.0f, 1.0f, 1.0f};
		inline float ambientStrength = 0.2f;
		inline bool debugDepthBufferLinear = false;
		inline bool debugNormalsViewSpace = false;

		inline Shader& getActiveShader();
		inline void drawUI();
	}

	namespace postprocessing
	{
		inline std::vector<PostProcessingStep> steps(1);
		inline void drawUI();
	}
	inline void drawUI();
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
	shaders::reload();
}

Shader& settings::rendering::getActiveShader()
{
	Shader& ret = [&]() -> Shader&{
		switch(active)
		{
			case type::gouraud:
				return resources::shaders::gouraud;
			case type::debugNormals:
				return resources::shaders::debugNormals;
			case type::debugTexCoords:
				return resources::shaders::debugTexCoords;
			case type::debugDepthBuffer:
				return resources::shaders::debugDepthBuffer;
			default:
				return resources::shaders::phong;
		}
	}();
	ret.use();
	switch(active)
	{
		case type::phong:
		case type::gouraud:
			ret.set("material.overrideDiffuse", overrideDiffuse);
			ret.set("material.overrideSpecular", overrideSpecular);
			ret.set("material.overrideDiffuseColor", overrideDiffuseColor);
			ret.set("material.overrideSpecularColor", overrideSpecularColor);
			ret.set("ambientStrength", ambientStrength);
			break;
		case type::debugDepthBuffer:
			ret.set("linearize", debugDepthBufferLinear);
			break;
		case type::debugNormals:
			ret.set("viewSpace", debugNormalsViewSpace);
			break;
	}
	return ret;
}

void resources::shaders::reload()
{
	resources::shaders::phong.reload();
	resources::shaders::gouraud.reload();
	resources::shaders::outline.reload();
	resources::shaders::debugNormals.reload();
	resources::shaders::debugTexCoords.reload();
	resources::shaders::debugDepthBuffer.reload();
	resources::shaders::light.reload();
	resources::shaders::skybox.reload();

	resources::shaders::passthrough.reload();
	resources::shaders::grayscale.reload();
	resources::shaders::invert.reload();
	resources::shaders::convolution.reload();
}

void resources::drawUI()
{
	if(ImGui::CollapsingHeader("Resources"))
	{
		ImGui::Indent();
		if(ImGui::CollapsingHeader("Models"))
		{
			if(models::nanosuit.drawUI())
				scene.update();
			if(models::sponza.drawUI())
				scene.update();
		}
		if(ImGui::CollapsingHeader("Cubemaps"))
		{
			if(cubemaps::skybox.drawUI())
				scene.update();
			if(cubemaps::mp_blizzard.drawUI())
				scene.update();
		}
		if(ImGui::Button("Reload Shaders"))
		{
			shaders::reload();
			scene.update();
		}
		ImGui::Unindent();
	}
	scene.drawUI();
}

void settings::drawUI()
{
	if(ImGui::CollapsingHeader("Settings"))
	{
		ImGui::Indent();
		rendering::drawUI();
		postprocessing::drawUI();
		ImGui::Unindent();
	}
}

void settings::rendering::drawUI()
{
	if(ImGui::CollapsingHeader("Rendering"))
	{
		ImGui::Indent();
		ImGui::Checkbox("Explicit Rendering", &explicitRendering);
		ImGui::SameLine();
		ImGui::Checkbox("V-Sync", &vsync);
		ImGui::SameLine();
		if(ImGui::Checkbox("Wireframe", &wireframe))
			resources::scene.update();

		ImGui::SameLine();
		if(ImGui::Checkbox("Depth Testing", &depthTesting))
			resources::scene.update();


		if(depthTesting && ImGui::CollapsingHeader("Depth Function"))
		{
			ImGui::Indent();
			if(ImGui::RadioButton("GL_ALWAYS", &depthFunction, GL_ALWAYS))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_NEVER", &depthFunction, GL_NEVER))
				resources::scene.update();

			if(ImGui::RadioButton("GL_LESS", &depthFunction, GL_LESS))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_EQUAL", &depthFunction, GL_EQUAL))
				resources::scene.update();

			if(ImGui::RadioButton("GL_LEQUAL", &depthFunction, GL_LEQUAL))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_GREATER", &depthFunction, GL_GREATER))
				resources::scene.update();

			if(ImGui::RadioButton("GL_NOTEQUAL", &depthFunction, GL_NOTEQUAL))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_GEQUAL", &depthFunction, GL_GEQUAL))
				resources::scene.update();

			ImGui::Unindent();
		}
		if(ImGui::Checkbox("Face Culling", &faceCulling))
			resources::scene.update();

		if(faceCulling)
		{
			ImGui::Indent();
			ImGui::Text("Face Culling Mode");
			if(ImGui::RadioButton("GL_BACK", &faceCullingMode, GL_BACK))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_FRONT", &faceCullingMode, GL_FRONT))
				resources::scene.update();

			if(ImGui::RadioButton("GL_FRONT_AND_BACK", &faceCullingMode, GL_FRONT_AND_BACK))
				resources::scene.update();

			ImGui::Text("Face Culling Ordering");
			if(ImGui::RadioButton("GL_CCW", &faceCullingOrdering, GL_CCW))
				resources::scene.update();

			ImGui::SameLine();
			if(ImGui::RadioButton("GL_CW ", &faceCullingOrdering, GL_CW))
				resources::scene.update();

			ImGui::Unindent();
		}
		if(ImGui::RadioButton("Phong", &active, type::phong))
			resources::scene.update();

		ImGui::SameLine();
		if(ImGui::RadioButton("Gouraud", &active, type::gouraud))
			resources::scene.update();

		ImGui::SameLine();
		if(ImGui::RadioButton("Normals", &active, type::debugNormals))
			resources::scene.update();

		ImGui::SameLine();
		if(ImGui::RadioButton("Texture Coordinates", &active, type::debugTexCoords))
			resources::scene.update();

		ImGui::SameLine();
		if(ImGui::RadioButton("Depth Buffer", &active, type::debugDepthBuffer))
			resources::scene.update();

		switch(active)
		{
			case type::phong:
			case type::gouraud:
				if(ImGui::Checkbox("Override Diffuse", &overrideDiffuse))
					resources::scene.update();

				if(overrideDiffuse)
				{
					ImGui::SameLine();
					if(ImGui::ColorEdit3("Diffuse", &overrideDiffuseColor.x, ImGuiColorEditFlags_NoInputs))
						resources::scene.update();

				}
				if(ImGui::Checkbox("Override Specular", &overrideSpecular))
					resources::scene.update();

				if(overrideSpecular)
				{
					ImGui::SameLine();
					if(ImGui::ColorEdit3("Specular", &overrideSpecularColor.x, ImGuiColorEditFlags_NoInputs))
						resources::scene.update();

				}
				if(ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f))
					resources::scene.update();

				break;
			case type::debugNormals:
				if(ImGui::Checkbox("View Space", &debugNormalsViewSpace))
					resources::scene.update();

				break;
			case type::debugTexCoords:

				break;
			case type::debugDepthBuffer:
				if(ImGui::Checkbox("Linearize", &debugDepthBufferLinear))
					resources::scene.update();

				break;
		}
		ImGui::Unindent();
	}
}

void settings::postprocessing::drawUI()
{
	if(ImGui::CollapsingHeader("Post Processing"))
	{
		if(ImGui::Button("Add step"))
			steps.emplace_back();
		int removeIdx = -1;
		for(int i = 0; i < steps.size(); i++)
		{
			if(ImGui::TreeNode(("Step " + std::to_string(i)).c_str()))
			{
				if(steps.size() > 1)
					if(ImGui::Button("Remove step"))
						removeIdx = i;
				steps[i].drawUI();
				ImGui::TreePop();
			}
		}
		if(removeIdx != -1)
			steps.erase(steps.begin() + removeIdx);
	}
}

void info::drawUI()
{
	ImGui::Text("Window size %i, %i", windowWidth, windowHeight);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}