#include "Camera.h"
#include <imgui.h>

void Camera::update()
{
	front = orientation.getDirectionVector();
	right = glm::normalize(glm::cross(-front, {0.0f, -1.0f, 0.0f}));
	up = glm::normalize(glm::cross(-front, right));
}

void Camera::use(Shader shader, float width, float height) const
{
	shader.set("projection", getProjectionMatrix(width, height));
	shader.set("view", getViewMatrix());
	shader.set("nearPlane", nearPlane);
	shader.set("farPlane", farPlane);
}

std::tuple<glm::mat4, glm::mat4> Camera::getMatrices(float width, float height) const
{
	return {getProjectionMatrix(width, height), getViewMatrix()};
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const
{
	if(projectionOrtho)
		return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	else
		return glm::perspective(glm::radians(fov), width / height, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position.position, position.position + front, up);
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