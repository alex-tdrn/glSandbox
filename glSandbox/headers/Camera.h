#pragma once
#include "Components.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tuple>

class Camera
{
private:
	bool projectionOrtho = false;
	Orientation orientation;
	Position position = {{0.0f, 0.0f, 3.0f}};
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float fov = 45.0f;
	glm::vec3 front{0.0f, 0.0f, -1.0f};
	glm::vec3 right = glm::normalize(glm::cross({0.0f, 1.0f, 0.0f}, -front));
	glm::vec3 up = glm::normalize(glm::cross(-front, right));

private:
	void update();

public:
	void use(Shader shader, int width, int height) const;
	std::tuple<glm::mat4, glm::mat4> getMatrices(int width, int height) const;
	glm::mat4 getProjectionMatrix(int width, int height) const;
	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;
	void setPosition(Position position);
	void setOrientation(Orientation orientation);
	void dolly(float amount);
	void pan(glm::vec2 amount);
	void adjustOrientation(float yaw, float pitch);
	[[nodiscard]]bool drawUI();
};