#include "Camera.h"
#include "Globals.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

Camera::Camera()
{
	localTranslation = {0.0f, 0.0f, 8.0f};
}

unsigned int Camera::ubo()
{
	static unsigned int ubo = [](){
		unsigned int ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		return ubo;
	}();

	return ubo;
}

void Camera::use() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(getProjectionMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(getViewMatrix()));
}

float Camera::getNearPlane() const
{
	return nearPlane;
}

float Camera::getFarPlane() const
{
	return farPlane;
}

std::tuple<glm::vec3, glm::vec3, glm::vec3> Camera::getCameraVectors() const
{
	static const glm::vec4 front{0.0f, 0.0f, -1.0f, 0.0f};
	static const glm::vec4 right{1.0f, 0.0f, 0.0f, 0.0f};
	static const glm::vec4 up{0.0f, 1.0f, 0.0f, 0.0f};
	glm::mat4 localTransformation = getLocalTransformation();
	glm::vec3 localFront = glm::normalize(glm::vec3(front * localTransformation));
	glm::vec3 localRight = glm::normalize(glm::cross(localFront, {0.0f, 1.0f, 0.0f}));
	glm::vec3 localUp = glm::normalize(glm::cross(localRight, localFront));
	return {std::move(localFront), std::move(localRight), std::move(localUp)};
}

glm::mat4 Camera::getProjectionMatrix() const
{
	if(projectionOrtho)
		return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	else
		return glm::perspective(glm::radians(fov), static_cast<float>(info::windowWidth) / info::windowHeight, nearPlane, farPlane);
}

glm::mat4 Camera::getLocalTransformation() const
{
	return glm::eulerAngleXY(glm::radians(localRotation.x), glm::radians(localRotation.y)) * glm::translate(glm::mat4(1.0f), localTranslation);
}
glm::mat4 Camera::getViewMatrix() const
{
	glm::mat4 viewMatrix = glm::eulerAngleXY(glm::radians(localRotation.x), glm::radians(localRotation.y)) * glm::translate(glm::mat4(1.0f), -localTranslation);
	if(parent != nullptr)
		return parent->getGlobalTransformation() * viewMatrix;
	return viewMatrix;
}

glm::vec3 Camera::getPosition() const
{
	auto[globalTranslation, globalRotation, globalScale] = decomposeTransformation(getGlobalTransformation());
	return globalTranslation;
}

void Camera::move(glm::vec3 amount)
{
	auto[localFront, localRight, localUp] = getCameraVectors();
	localTranslation += localFront * amount.z + localRight * amount.x + localUp * amount.y;
}

void Camera::rotate(float yawAmount, float pitchAmount)
{
	localRotation.y += yawAmount;
	localRotation.y -= int(localRotation.y) / 360 * 360;
	if(localRotation.y < 0)
		localRotation.y += 360;
	localRotation.x += pitchAmount;
	if(localRotation.x > 89.0f)
		localRotation.x = 89.0f;
	else if(localRotation.x < -89.0f)
		localRotation.x = -89.0f;
}

void Camera::drawUI()
{
	/*IDGuard idGuard{this};

	bool valueChanged = false;
	if(ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Indent();

		ImGui::Text("Projection");
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("Perspective", reinterpret_cast<int*>(&projectionOrtho), 0);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("Orthographic", reinterpret_cast<int*>(&projectionOrtho), 1);
		valueChanged |= ImGui::DragFloat("FOV", &fov, 0.1f);
		valueChanged |= ImGui::DragFloat("Near Plane", &nearPlane, 0.01f);
		valueChanged |= ImGui::DragFloat("Far Plane", &farPlane, 0.1f);
		valueChanged |= orientation.drawUI();
		valueChanged |= position.drawUI();
		ImGui::Text("Front: (%.2f, %.2f, %.2f)", front.x, front.y, front.z);
		ImGui::Text("Right: (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
		ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);

		ImGui::Unindent();
	}
	if(valueChanged) 
		update();
	return valueChanged;*/
}