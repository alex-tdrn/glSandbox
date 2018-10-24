#pragma once
#include "TransformedNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc\quaternion.hpp>
#include <tuple>

class Camera final : public Transformed<Translation, Rotation>
{
private:
	bool visualizeFrustum = true;
	bool projectionOrtho = false;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float fov = 45.0f;
	float orthoScale = 0.005f;

public:
	Camera();

private:
	static unsigned int ubo();

protected:
	std::string getNamePrefix() const override;

public:
	void use() const;
	glm::mat4 getProjectionMatrix() const;
	glm::mat4 getViewMatrix() const;
	float getNearPlane() const;
	float getFarPlane() const;
	bool getVisualizeFrustum() const;
	void move(glm::vec3 amount);
	void drawUI() override;

};