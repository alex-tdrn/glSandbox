#include "Camera.h"
#include "Globals.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

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

void Camera::update()
{
	/*front = orientation.getDirectionVector();
	right = glm::normalize(glm::cross(-front, {0.0f, -1.0f, 0.0f}));
	up = glm::normalize(glm::cross(-front, right));*/
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
		return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	else
		return glm::perspective(glm::radians(fov), static_cast<float>(info::windowWidth) / info::windowHeight, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::translate(glm::mat4(1.0f), -position.position) * glm::eulerAngleXY(glm::radians(pitch), glm::radians(yaw));
	//return glm::lookAt(position.position, position.position + front, up);
}

glm::vec3 Camera::getPosition() const
{
	return position.position;
}

void Camera::setPosition(Position position)
{
	this->position = position;
	update();
}

void Camera::dolly(float amount)
{
	position.position += front * amount;
	update();
}

void Camera::pan(glm::vec2 amount)
{
	position.position += right * amount.x;
	position.position += up * amount.y;
	update();
}

void Camera::adjustOrientation(float yawAmount, float pitchAmount)
{
	yaw += yawAmount;
	yaw -= int(yaw) / 360 * 360;
	if(yaw < 0)
		yaw += 360;
	pitch += pitchAmount;
	if(pitch > 89.0f)
		pitch = 89.0f;
	else if(pitch < -89.0f)
		pitch = -89.0f;

	glm::quat diff{glm::vec3{glm::radians(pitch), glm::radians(yaw), 0}};
	orientation = glm::normalize(diff * orientation);
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