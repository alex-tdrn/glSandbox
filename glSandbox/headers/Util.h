#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui.h>
#include <string>
#include <tuple>
#include <algorithm>

class Bounds
{
private:
	glm::vec3 min{0};
	glm::vec3 max{0};

public:
	Bounds() = default;
	Bounds(glm::vec3 point1, glm::vec3 point2)
		: min(point1), max(point2)
	{
		normalize();
	}

	Bounds& operator*(glm::mat4 const& rhs)
	{
		glm::vec4 min{this->min, 1.0f};
		glm::vec4 max{this->max, 1.0f};
		min = rhs * min;
		max = rhs * max;
		normalize();
		return *this;
	}
	Bounds& operator+(Bounds const& rhs)
	{
		glm::vec3 min;
		glm::vec3 max;
		for(auto i = 0; i < 3; i++)
		{
			min[i] = std::min(this->min[i], rhs.min[i]);
			max[i] = std::max(this->max[i], rhs.max[i]);
		}
		this->min = min;
		this->max = max;
		normalize();
		return *this;
	}
	Bounds& operator*=(glm::mat4 const& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Bounds operator+=(Bounds const& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

private:
	void normalize()
	{
		glm::vec3 realMin;
		glm::vec3 realMax;
		for(auto i = 0; i < 3; i++)
		{
			realMin[i] = std::min(min[i], max[i]);
			realMax[i] = std::max(min[i], max[i]);
		}
		min = realMin;
		max = realMax;
	}

public:
	std::pair<glm::vec3, glm::vec3> getValues() const
	{
		return {min, max};
	}
};

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

inline std::tuple<glm::vec3, glm::quat, glm::vec3> decomposeTransformation(glm::mat4 const& transformation)
{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, scale, rotation, translation, skew, perspective);

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
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.1f);
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