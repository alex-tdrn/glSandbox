#pragma once
#include "Components.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc\quaternion.hpp>
#include <tuple>

class Camera
{
private:
	bool projectionOrtho = false;
	glm::quat orientation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 1, 0));
	float yaw = 0.0f;
	float pitch = 0.0f;
	Position position = {{0.0f, 0.0f, 8.0f}};
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float fov = 45.0f;
	glm::vec3 front{0.0f, 0.0f, -1.0f};
	glm::vec3 right = glm::normalize(glm::cross({0.0f, 1.0f, 0.0f}, -front));
	glm::vec3 up = glm::normalize(glm::cross(-front, right));

private:
	void update();
	static unsigned int ubo();

public:
	void use() const;
	glm::mat4 getProjectionMatrix() const;
	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;
	float getNearPlane() const;
	float getFarPlane() const;
	void setPosition(Position position);
	//void setOrientation(Orientation orientation);
	void dolly(float amount);
	void pan(glm::vec2 amount);
	void adjustOrientation(float yaw, float pitch);
	void drawUI();
};