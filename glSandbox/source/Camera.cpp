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

std::string const& Camera::getName() const
{
	return name.get();
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
	glm::mat4 localCameraTransformation = glm::eulerAngleXY(glm::radians(localRotation.x), glm::radians(localRotation.y)) * glm::translate(glm::mat4(1.0f), localTranslation);
	glm::vec3 localFront = glm::normalize(glm::vec3(front * localCameraTransformation));
	glm::vec3 localRight = glm::normalize(glm::cross(localFront, {0.0f, 1.0f, 0.0f}));
	glm::vec3 localUp = glm::normalize(glm::cross(localRight, localFront));
	return {std::move(localFront), std::move(localRight), std::move(localUp)};
}

glm::mat4 Camera::getProjectionMatrix() const
{
	if(projectionOrtho)
		return glm::ortho(-info::windowWidth * 0.5f * orthoScale, +info::windowWidth * 0.5f * orthoScale, -info::windowHeight* 0.5f * orthoScale, +info::windowHeight* 0.5f * orthoScale, nearPlane, farPlane);
	else
		return glm::perspective(glm::radians(fov), static_cast<float>(info::windowWidth) / info::windowHeight, nearPlane, farPlane);
}

glm::mat4 Camera::getLocalTransformation() const
{
	return glm::translate(glm::mat4(1.0f), localTranslation) * glm::eulerAngleYX(glm::radians(-localRotation.y), glm::radians(-localRotation.x));
}

glm::mat4 Camera::getGlobalTransformation() const
{
	glm::mat4 localTransformation = getLocalTransformation();

	if(parent != nullptr)
		return removeScaling(parent->getGlobalTransformation()) * localTransformation;
	return localTransformation;
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::mat4 viewMatrix = glm::eulerAngleXY(glm::radians(localRotation.x), glm::radians(localRotation.y)) * glm::translate(glm::mat4(1.0f), -localTranslation);
	if(parent != nullptr)
	{
		auto globalTransformation = parent->getGlobalTransformation();
		return  viewMatrix * glm::inverse(extractRotationMatrix(globalTransformation)) * glm::translate(glm::mat4(1.0f), -extractTranslationVector(globalTransformation));
	}
	return viewMatrix;
}

glm::vec3 Camera::getPosition() const
{
	return extractTranslationVector(getGlobalTransformation());
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
	Node::drawUI();
	IDGuard idGuard{this};
	ImGui::BeginChild("Camera", {ImGui::GetTextLineHeightWithSpacing() * 22, ImGui::GetTextLineHeightWithSpacing() * 9});
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Projection");
	ImGui::SameLine();
	ImGui::RadioButton("Perspective", reinterpret_cast<int*>(&projectionOrtho), 0);
	ImGui::SameLine();
	ImGui::RadioButton("Orthographic", reinterpret_cast<int*>(&projectionOrtho), 1);
	if(projectionOrtho)
		ImGui::DragFloat("Scale", &orthoScale, 0.0001f);
	else
		ImGui::DragFloat("FOV", &fov, 0.1f);
	ImGui::DragFloat("Near Plane", &nearPlane, 0.01f);
	ImGui::DragFloat("Far Plane", &farPlane, 0.1f);
	auto[localFront, localRight, localUp] = getCameraVectors();
	ImGui::Text("Front: (%.2f, %.2f, %.2f)", localFront.x, localFront.y, localFront.z);
	ImGui::Text("Right: (%.2f, %.2f, %.2f)", localRight.x, localRight.y, localRight.z);
	ImGui::Text("Up: (%.2f, %.2f, %.2f)", localUp.x, localUp.y, localUp.z);
	ImGui::EndChild();
}