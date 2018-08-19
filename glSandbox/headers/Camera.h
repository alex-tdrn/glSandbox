#pragma once
#include "Node.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc\quaternion.hpp>
#include <tuple>

class Camera : public Node
{
private:
	bool projectionOrtho = false;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float fov = 45.0f;

public:
	Camera();
	virtual ~Camera() = default;

private:
	static unsigned int ubo();
	std::tuple<glm::vec3, glm::vec3, glm::vec3> getCameraVectors() const;
	glm::mat4 getProjectionMatrix() const;

public:
	void use() const;
	glm::mat4 getLocalTransformation() const override;
	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;
	float getNearPlane() const;
	float getFarPlane() const;
	void move(glm::vec3 amount);
	void rotate(float yaw, float pitch);
	void drawUI();

};