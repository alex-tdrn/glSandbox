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
}

glm::vec3 const DirectionalLight::getDirection() const
{
	return {};
}


bool DirectionalLight::drawUI()
{
	return true;
}

void PointLight::setPosition(glm::vec3 position)
{
}

glm::vec3 const& PointLight::getPosition() const
{
	return {};
}

bool PointLight::drawUI()
{
	return true;

}

void SpotLight::setOrientation(glm::vec3 orientation)
{
}

void SpotLight::setPosition(glm::vec3 position)
{
}

void SpotLight::setCutoff(float inner, float outer)
{
	this->innerCutoff = inner;
	this->outerCutoff = outer;
}

glm::vec3 const& SpotLight::getPosition() const
{
	return {};

}

glm::vec3 const SpotLight::getDirection() const
{
	return {};

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
	return true;

}