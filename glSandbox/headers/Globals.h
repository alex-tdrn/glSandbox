#pragma once

#include "Resources.h"
#include "PostProcessingStep.h"
#include "Shader.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>


namespace info
{
	inline int windowWidth = 1920;
	inline int windowHeight = 960;
	inline float aspect()
	{
		return float(windowWidth) / windowHeight;
	}
	inline bool rendering = false;
	inline void drawUI(bool* open);
}

namespace settings
{
	static std::unique_ptr<Renderer> mainRenderer;

	namespace rendering
	{
		inline bool vsync = true;
		inline bool gammaCorrection = true;
		enum tonemappingType
		{
			none,
			reinhard,
			uncharted2,
			hejl_burgess_dawson
		};
		inline int tonemapping = tonemappingType::none;
		inline float exposure = 1.0f;
		inline float gammaExponent = 2.2f;
	}

	namespace postprocessing
	{
		inline std::vector<PostProcessingStep> steps(1);
		inline void drawUI(bool* open);
	}
}



void settings::postprocessing::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Post Processing", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

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

	ImGui::End();
}

void info::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Stats", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Window size %i, %i", windowWidth, windowHeight);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Renderer: ");
	ImGui::SameLine();
	ImGui::Text(rendering ? "active" : "waiting");
	ImGui::End();
}