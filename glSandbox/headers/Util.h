#pragma once
#include "glm/glm.hpp"
#include <imgui.h>
#include <string>
#include <tuple>

class IDGuard
{
public:
	IDGuard() = delete;
	IDGuard(void* ID)
	{
		ImGui::PushID(ID);
	}
	~IDGuard()
	{
		ImGui::PopID();
	}
};

inline void wrapAround(float& x, float const min, float const max)
{
	float const diff = max - min;
	while(x < min)
		x += diff;
	while(x > max)
		x -= diff;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y)
{
	bool valueChanged = false;
	ImGui::Button(title);
	if(ImGui::IsItemActive())
	{
		ImVec2 mouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);
		x += mouseDelta.x  * sensitivity;
		y -= mouseDelta.y * sensitivity;
		valueChanged |= mouseDelta.x != 0 || mouseDelta.y != 0;
	}
	return valueChanged;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y, float const min, float const max)
{
	bool ret = drag2(title, sensitivity, x, y);
	wrapAround(x, min, max);
	wrapAround(y, min, max);
	return ret;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y, float const xmin, float const xmax, const float ymin, const float ymax)
{
	bool ret = drag2(title, sensitivity, x, y);
	wrapAround(x, xmin, xmax);
	wrapAround(y, ymin, ymax);
	return ret;
}

inline std::tuple<glm::vec3, glm::vec3, glm::vec3> decomposeTransformation(glm::mat4 const& t)
{
	glm::vec3 translation{t[3][0], t[3][1], t[3][2]};
	glm::vec3 scale{glm::length(t[0]), glm::length(t[1]), glm::length(t[2])};
	glm::mat4 rotationMatrix{1.0f};
	rotationMatrix[0] = t[0] / scale.x;
	rotationMatrix[1] = t[1] / scale.y;
	rotationMatrix[2] = t[2] / scale.z;
	glm::vec3 rotation{};
	return {translation, rotation, scale};
}

inline void drawMatrix(glm::mat4 const& matrix)
{
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.2f);
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			ImGui::Text("%.2f", matrix[col][row]);
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::PopItemWidth();
}

inline bool drawEditableMatrix(glm::mat4& matrix)
{
	bool valueChanged = false;
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.2f);
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			valueChanged |= ImGui::DragFloat((std::string("###") + std::to_string(row) + std::to_string(col)).data(), &matrix[col][row], 0.01f, 0,0, "%.2f");
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::PopItemWidth();
	return valueChanged;
}