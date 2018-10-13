#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <string_view>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <array>
#include <vector>
#include <memory>

template <typename...>
struct is_one_of
{
	static constexpr bool value = false;
};

template <typename F, typename S, typename... T>
struct is_one_of<F, S, T...>
{
	static constexpr bool value =
		std::is_same<F, S>::value || is_one_of<F, T...>::value;
};

class LifetimeLogger
{
public:
	LifetimeLogger()
	{
		std::cout << "Constructor " << this << '\n';
	}
	LifetimeLogger(LifetimeLogger const&)
	{
		std::cout << "Copy Constructor " << this << '\n';
	}
	LifetimeLogger(LifetimeLogger&&)
	{
		std::cout << "Move Constructor " << this << '\n';
	}
	LifetimeLogger& operator=(LifetimeLogger const&)
	{
		std::cout << "Copy Assignment " << this << '\n';
	}
	LifetimeLogger& operator=(LifetimeLogger&&)
	{
		std::cout << "Move Assignment " << this << '\n';
	}
	~LifetimeLogger()
	{
		std::cout << "Destructor " << this << '\n';
	}
};

class Bounds
{
private:
	bool _empty = false;
	glm::vec3 min{0};
	glm::vec3 max{0};

public:
	Bounds()
		:_empty{true}
	{

	}
	Bounds(glm::vec3 point1, glm::vec3 point2)
		: min{point1}, max{point2}
	{
		normalize();
	}
	Bounds(glm::vec3 point)
		: min{point}, max{point}
	{
	}
	Bounds(float const point1[3], float const point2[3])
		: min{point1[0], point1[1], point1[2]}, max{point2[0], point2[1], point2[2]}
	{
		normalize();
	}
	Bounds(float const point[3])
		: min{point[0], point[1], point[2]}, max{min}
	{
	}
	Bounds& operator*=(glm::mat4 const& rhs)
	{
		if(!_empty)
		{
			std::array<glm::vec4, 8> corners;
			int idx = 0;
			for(int x = 0; x <= 1; x++)
			{
				for(int y = 0; y <= 1; y++)
				{
					for(int z = 0; z <= 1; z++)
					{
						corners[idx++] = glm::vec4{
							x ? min.x : max.x,
							y ? min.y : max.y,
							z ? min.z : max.z,
							1.0f};
					}
				}
			}
			bool initMinMax = false;
			for(auto& corner : corners)
			{
				glm::vec4 transformedCorner = rhs * corner;
				if(!initMinMax)
				{
					min = transformedCorner;
					max = transformedCorner;
					initMinMax = true;
				}
				for(int i = 0; i < 3; i++)
				{
					if(std::abs(transformedCorner[i]) > 2.0f)
					{
						volatile int a = 2;
					}
					min[i] = std::min(min[i], transformedCorner[i]);
					max[i] = std::max(max[i], transformedCorner[i]);
				}
			}

			normalize();
		}
		return *this;
	}
	Bounds& operator+=(Bounds const& rhs)
	{
		if(_empty && !rhs._empty)
		{
			_empty = false;
			*this = rhs;
		}
		else if(!_empty && !rhs._empty)
		{
			for(auto i = 0; i < 3; i++)
			{
				this->min[i] = std::min(this->min[i], rhs.min[i]);
				this->max[i] = std::max(this->max[i], rhs.max[i]);
			}
			normalize();
		}
		return *this;
	}
	Bounds& operator+=(glm::vec3 t)
	{
		min += t;
		max += t;
		return *this;
	}
	Bounds operator+(glm::vec3 t)
	{
		auto ret(*this);
		return ret += t;
	}
	friend Bounds operator*(Bounds const& lhs, glm::mat4 const& rhs);
	friend Bounds operator+(Bounds const& lhs, Bounds const& rhs);

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
	bool empty() const
	{
		return _empty;
	}
	std::pair<glm::vec3, glm::vec3> getValues() const
	{
		return {min, max};
	}
	glm::vec3 getCenter() const
	{
		return (min + max) * 0.5f;
	}
	glm::mat4 getTransformation() const
	{
		glm::vec3 translation = -getCenter();
		auto aux{*this};
		aux += translation;

		glm::vec3 scale;
		for(int i = 0; i < 3; i++)
			scale[i] = std::max(std::abs(aux.min[i]), std::abs(aux.max[i]));
		glm::mat4 t = glm::translate(glm::mat4{1.0f}, -translation);
		glm::mat4 s = glm::scale(glm::mat4{1.0f}, glm::vec3{scale});
		return t * s;
	}
};

inline Bounds operator*(Bounds const& lhs, glm::mat4 const& rhs)
{
	if(lhs._empty)
		return lhs;
	Bounds ret = lhs;
	ret *= rhs;
	return ret;
}

inline Bounds operator+(Bounds const& lhs, Bounds const& rhs)
{
	if(lhs._empty && !rhs._empty)
		return rhs;
	if(!lhs._empty && rhs._empty)
		return lhs;
	if(lhs._empty && rhs._empty)
		return {};

	glm::vec3 min;
	glm::vec3 max;
	for(auto i = 0; i < 3; i++)
	{
		min[i] = std::min(lhs.min[i], rhs.min[i]);
		max[i] = std::max(lhs.max[i], rhs.max[i]);
	}
	return {min, max};
}

class IDGuard
{
public:
	IDGuard() = delete;
	IDGuard(int* ID)
	{
		ImGui::PushID(ID);
	}
	IDGuard(char const* ID)
	{
		ImGui::PushID(ID);
	}
	IDGuard(void* ID)
	{
		ImGui::PushID(ID);
	}
	~IDGuard()
	{
		ImGui::PopID();
	}
};

template <typename T>
T* chooseFromCombo(T* currentValue, std::vector<std::unique_ptr<T>> const& possibleValues, bool nullable = false)
{
	IDGuard idGuard{currentValue};
	if(ImGui::BeginCombo("###Combo", currentValue ? currentValue->name.get().data() : "None"))
	{
		bool isSelected = !currentValue;
		if(nullable)
		{
			if(ImGui::Selectable("None", &isSelected))
				currentValue = nullptr;
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::Separator();
		}
		int id = 0;
		for(auto& v : possibleValues)
		{
			ImGui::PushID(id++);
			isSelected = currentValue == v.get();
			if(ImGui::Selectable(v->name.get().data(), &isSelected))
				currentValue = v.get();
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return currentValue;
}

inline glm::mat4 composeTransformation(glm::vec3 const& translation, glm::vec3 const& rotation, glm::vec3 const& scale)
{
	glm::mat4 Tr = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 Ro = glm::eulerAngleZYX(glm::radians(rotation.z), glm::radians(rotation.y), glm::radians(rotation.x));
	glm::mat4 Sc = glm::scale(glm::mat4(1.0f), scale);
	return Tr * Ro * Sc;
}

inline std::tuple<glm::vec3, glm::quat, glm::vec3> decomposeTransformation(glm::mat4 const& matrix)
{
	glm::vec3 scale;
	glm::mat4 rot{1.0f};
	for(int i = 0; i < 3; i++)
	{
		scale[i] = glm::length(matrix[i]);
		rot[i] = matrix[i] / scale[i];
	}
	return {matrix[3], glm::quat_cast(rot), std::move(scale)};
}
inline glm::vec3 extractTranslationVector(glm::mat4 const& matrix)
{
	return matrix[3];
}

inline glm::mat4 extractRotationMatrix(glm::mat4 matrix)
{
	matrix[3] = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
	for(int i = 0; i < 3; i++)
		matrix[i] = glm::normalize(matrix[i]);

	return matrix;
}

inline glm::mat4 removeScaling(glm::mat4 matrix)
{
	for(int i = 0; i < 3; i++)
		matrix[i] = glm::normalize(matrix[i]);
	return matrix;
}

inline std::string glEnumToString(GLenum e)
{
	switch(e)
	{
		case GL_POINTS:
			return "GL_POINTS";
		case GL_LINES:
			return "GL_LINES";
		case GL_LINE_LOOP:
			return "GL_LINE_LOOP";
		case GL_TRIANGLE_FAN:
			return "GL_TRIANGLE_FAN";
		case GL_TRIANGLE_STRIP:
			return "GL_TRIANGLE_STRIP";
		case GL_TRIANGLES:
			return "GL_TRIANGLES";
		case GL_BYTE:
			return "GL_BYTE";
		case GL_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";
		case GL_SHORT:
			return "GL_SHORT";
		case GL_UNSIGNED_SHORT:
			return "GL_UNSIGNED_SHORT";
		case GL_FLOAT:
			return "GL_FLOAT";
		case GL_UNSIGNED_INT:
			return "GL_UNSIGNED_INT";
	}
	return "Unrecognized GL enum";
}