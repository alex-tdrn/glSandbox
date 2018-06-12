#include "Camera.h"
#include "Globals.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

void Camera::init() const
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
}

void Camera::update()
{
	front = orientation.getDirectionVector();
	right = glm::normalize(glm::cross(-front, {0.0f, -1.0f, 0.0f}));
	up = glm::normalize(glm::cross(-front, right));
}

void Camera::use() const
{
	if(!initialized)
		init();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
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
	return glm::lookAt(position.position, position.position + front, up);
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

void Camera::setOrientation(Orientation orientation)
{
	this->orientation = orientation;
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
	orientation.yaw += yawAmount;
	orientation.yaw -= int(orientation.yaw) / 360 * 360;
	if(orientation.yaw < 0)
		orientation.yaw += 360;
	orientation.pitch += pitchAmount;
	if(orientation.pitch > 89.0f)
		orientation.pitch = 89.0f;
	else if(orientation.pitch < -89.0f)
		orientation.pitch = -89.0f;
	update();
}

bool Camera::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	if(ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Indent();

		ImGui::Text("Projection");
		ImGui::SameLine();
		if(ImGui::RadioButton("Perspective", reinterpret_cast<int*>(&projectionOrtho), 0))
			valueChanged = true;
		ImGui::SameLine();
		if(ImGui::RadioButton("Orthographic", reinterpret_cast<int*>(&projectionOrtho), 1))
			valueChanged = true;

		if(ImGui::DragFloat("FOV", &fov, 0.1f))
			valueChanged = true;

		if(ImGui::DragFloat("Near Plane", &nearPlane, 0.01f))
			valueChanged = true;

		if(ImGui::DragFloat("Far Plane", &farPlane, 0.1f))
			valueChanged = true;

		if(orientation.drawUI())
			valueChanged = true;
		if(position.drawUI())
			valueChanged = true;
		ImGui::Text("Front: (%.2f, %.2f, %.2f)", front.x, front.y, front.z);
		ImGui::Text("Right: (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
		ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);

		ImGui::Unindent();
	}
	if(valueChanged) 
		update();
	return valueChanged;
}