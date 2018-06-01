#include "Lights.h"
#include "imgui.h"

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

bool Light::drawUI()
{
	bool valueChanged = false;
	if(ImGui::Checkbox("Enabled", &enabled))
		valueChanged = true;
	if(ImGui::ColorEdit3("Color", &Light::color.x, ImGuiColorEditFlags_NoInputs))
		valueChanged = true;

	return valueChanged;
}

void DirectionalLight::setOrientation(glm::vec3 orientation)
{
	this->orientation.yaw = orientation.x;
	this->orientation.pitch = orientation.y;
	this->orientation.roll = orientation.z;
}

glm::vec3 const& DirectionalLight::getDirection() const
{
	return orientation.getDirectionVector();
}


bool DirectionalLight::drawUI()
{
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

float PointLight::getConstant() const
{
	return constant;
}

float PointLight::getLinear() const
{
	return linear;
}

float PointLight::getQuadratic() const
{
	return quadratic;
}

bool PointLight::drawUI()
{
	bool valueChanged = false;
	if(Light::drawUI())
		valueChanged = true;

	if(ImGui::DragFloat("Constant", &constant, 0.001f))
		valueChanged = true;

	if(ImGui::DragFloat("Linear", &linear, 0.001f))
		valueChanged = true;

	if(ImGui::DragFloat("Quadratic", &quadratic, 0.001f))
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

glm::vec3 const& SpotLight::getDirection() const
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