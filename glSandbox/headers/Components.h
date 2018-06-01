#pragma once
#include <glm/glm.hpp>

class Position
{
public:
	glm::vec3 position{0.0f, 0.0f, 0.0f};

	bool drawUI();
};

class Orientation
{
public:
	float yaw = 270.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	glm::vec3 const& getDirectionVector() const;
	bool drawUI();
};

class Scale
{
public:
	glm::vec3 scale{1.0f, 1.0f, 1.0f};

	bool drawUI();
};