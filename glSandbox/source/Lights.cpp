#include "Lights.h"
#include "imgui.h"
#include "Util.h"

bool Light::isEnabled() const
{
	return enabled;
}

void Light::enable()
{
	enabled = true;
}

void Light::disable()
{
	enabled = false;
}

void Light::setColor(glm::vec3 color)
{
	this->color = color;
}

glm::vec3 const& Light::getColor() const
{
	return color;
}

void Light::setIntensity(float intensity)
{
	this->intensity = intensity;
}

float Light::getIntensity() const
{
	return intensity;
}

bool Light::drawUI()
{
	bool valueChanged = false;
	if(ImGui::Checkbox("Enabled", &enabled))
		valueChanged = true;
	if(ImGui::ColorEdit3("Color", &Light::color.x, ImGuiColorEditFlags_NoInputs))
		valueChanged = true;
	if(ImGui::DragFloat("Intensity", &intensity, 0.5f))
		valueChanged = true;

	return valueChanged;
}

void DirectionalLight::setOrientation(glm::vec3 orientation)
{
	this->orientation.yaw = orientation.x;
	this->orientation.pitch = orientation.y;
	this->orientation.roll = orientation.z;
}

glm::vec3 const DirectionalLight::getDirection() const
{
	return orientation.getDirectionVector();
}


bool DirectionalLight::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	if(Light::drawUI())
		valueChanged = true;

	if(orientation.drawUI())
		valueChanged = true;

	return valueChanged;
}

void PointLight::setPosition(glm::vec3 position)
{
	this->position.position = position;
}

glm::vec3 const& PointLight::getPosition() const
{
	return position.position;
}

bool PointLight::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	if(Light::drawUI())
		valueChanged = true;

	if(position.drawUI())
		valueChanged = true;
	return valueChanged;
}

void SpotLight::setOrientation(glm::vec3 orientation)
{
	this->orientation.yaw = orientation.x;
	this->orientation.pitch = orientation.y;
	this->orientation.roll = orientation.z;
}

void SpotLight::setPosition(glm::vec3 position)
{
	this->position.position = position;
}


glm::vec3 const& SpotLight::getPosition() const
{
	return position.position;
}

glm::vec3 const SpotLight::getDirection() const
{
	return orientation.getDirectionVector();
}

float SpotLight::getInnerCutoff() const
{
	return innerCutoff;
}

float SpotLight::getOuterCutoff() const
{
	return outerCutoff;
}

bool SpotLight::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	if(Light::drawUI())
		valueChanged = true;

	if(ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.02f))
		valueChanged = true;

	if(ImGui::DragFloat("Outer Cutoff", &outerCutoff, 0.02f))
		valueChanged = true;

	if(orientation.drawUI())
		valueChanged = true;

	if(position.drawUI())
		valueChanged = true;
	return valueChanged;
}