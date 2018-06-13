#include "Components.h"
#include "Util.h"
#include <imgui.h>

bool Position::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	valueChanged |= ImGui::DragFloat3("Position", &position.x, 0.2f);
	valueChanged |= drag2("XY", 0.001f, position.x, position.y);
	ImGui::SameLine();
	valueChanged |= drag2("XZ", 0.001f, position.x, position.z);
	ImGui::SameLine();
	valueChanged |= drag2("ZY", 0.001f, position.z, position.y);
	return valueChanged;
}

glm::vec3 const Orientation::getDirectionVector() const
{
	glm::vec3 direction;
	const float rPitch = glm::radians(pitch);
	const float rYaw = glm::radians(yaw);
	direction.x = cos(rPitch) * cos(rYaw);
	direction.y = sin(rPitch);
	direction.z = cos(rPitch) * sin(rYaw);
	return glm::normalize(direction);
}

bool Orientation::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	valueChanged |= ImGui::SliderFloat("Yaw", &yaw, 0.0f, 359.9f);
	valueChanged |= ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f);
	valueChanged |= ImGui::SliderFloat("Roll", &roll, 0.0f, 359.9f);
	valueChanged |= drag2("YP", 0.01f, yaw, pitch, 0.0f, 359.9f, -90.0f, 90.0f);
	return valueChanged;
}

bool Scale::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	valueChanged |= ImGui::DragFloat3("Scale", &scale.x, 0.01f);
	return valueChanged;
}