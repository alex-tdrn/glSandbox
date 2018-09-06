#pragma once
#include "TransformedNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc\quaternion.hpp>
#include <tuple>

class Camera : public Transformed<Translation, Rotation>
{
private:
	Name<Camera> name{"camera"};
	bool projectionOrtho = false;
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
	glm::mat4 getViewMatrix() const;
	float getNearPlane() const;
	float getFarPlane() const;
	void move(glm::vec3 amount);
	void drawUI() override;

};