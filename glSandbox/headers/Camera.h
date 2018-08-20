#pragma once
#include "Node.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc\quaternion.hpp>
#include <tuple>

class Camera : public Node
{
private:
	Name<Camera> name{"camera"};
	bool projectionOrtho = false;
	bool orbital = true;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float fov = 45.0f;
	float orthoScale = 0.005f;

public:
	Camera();
	virtual ~Camera() = default;

private:
	static unsigned int ubo();
	glm::mat4 getProjectionMatrix() const;

public:
	void use() const;
	std::string const& getName() const override;
	glm::mat4 getLocalTransformation() const override;
	glm::mat4 getGlobalTransformation() const override;
	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;
	float getNearPlane() const;
	float getFarPlane() const;
	void move(glm::vec3 amount);
	void rotate(float yaw, float pitch);
	void drawUI() override;

};