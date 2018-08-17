#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <iostream>

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
		assert(!_empty);
		glm::vec4 min{this->min, 1.0f};
		glm::vec4 max{this->max, 1.0f};
		this->min = rhs * min;
		this->max = rhs * max;
		normalize();
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
};

inline Bounds operator*(Bounds const& lhs, glm::mat4 const& rhs)
{
	assert(!lhs._empty);
	glm::vec4 min{lhs.min, 1.0f};
	glm::vec4 max{lhs.max, 1.0f};
	return {rhs * min, rhs * max};
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

