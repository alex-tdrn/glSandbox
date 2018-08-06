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
	valueChanged |= ImGui::Checkbox("Enabled", &enabled);
	valueChanged |= ImGui::ColorEdit3("Color", &Light::color.x, ImGuiColorEditFlags_NoInputs);
	valueChanged |= ImGui::DragFloat("Intensity", &intensity, 0.1f);
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
	valueChanged |= Light::drawUI();
	valueChanged |= orientation.drawUI();
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
	valueChanged |= Light::drawUI();
	valueChanged |= position.drawUI();
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

void SpotLight::setCutoff(float inner, float outer)
{
	this->innerCutoff = inner;
	this->outerCutoff = outer;
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
	valueChanged |= Light::drawUI();
	valueChanged |= ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.02f);
	valueChanged |= ImGui::DragFloat("Outer Cutoff", &outerCutoff, 0.02f);
	valueChanged |= orientation.drawUI();
	valueChanged |= position.drawUI();
	return valueChanged;
}