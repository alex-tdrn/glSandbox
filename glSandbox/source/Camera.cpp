#include "Camera.h"
#include "Globals.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

Camera::Camera()
{
	setLimitPitch(true);
	setLocalTranslation(glm::vec3{0.0f, 3.0f, 10.0f});
	setLocalRotation(glm::vec3{-15.0f, 0.0f, 0.0f});
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

glm::mat4 Camera::getProjectionMatrix() const
{
	if(projectionOrtho)
		return glm::ortho(-info::windowWidth * 0.5f * orthoScale, +info::windowWidth * 0.5f * orthoScale, -info::windowHeight* 0.5f * orthoScale, +info::windowHeight* 0.5f * orthoScale, nearPlane, farPlane);
	else
		return glm::perspective(glm::radians(fov), static_cast<float>(info::windowWidth) / info::windowHeight, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::inverse(getGlobalTransformation());
}

bool Camera::getVisualizeFrustum() const
{
	return visualizeFrustum;
}

void Camera::move(glm::vec3 amount)
{
	amount[2] *= -1;
	translate(glm::mat3(getLocalRotationMatrix()) * amount);
}

void Camera::drawUI()
{
	Transformed<Translation, Rotation>::drawUI();
	IDGuard idGuard{this};
	ImGui::BeginChild("Camera", {ImGui::GetTextLineHeightWithSpacing() * 22, ImGui::GetTextLineHeightWithSpacing() * 10});
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
	ImGui::Checkbox("Visualize Frustum", &visualizeFrustum);
	ImGui::EndChild();
}