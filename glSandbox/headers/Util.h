#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <iostream>

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