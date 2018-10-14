#include "Lights.h"
#include "imgui.h"
#include "Globals.h"
#include "Shader.h"

#include <GLFW\glfw3.h>
#include <glad/glad.h>

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

void Light::use(std::string const& prefix, Shader& shader, bool flash) const
{
	shader.set(prefix + "color", getColor());
	if(flash)
		shader.set(prefix + "intensity", static_cast<float>(((std::sin(glfwGetTime()*10) + 1.0f) / 2.0f) * getIntensity()));
	else
		shader.set(prefix + "intensity", getIntensity());
}

void Light::drawUI()
{
	ImGui::ColorEdit3("Color", &Light::color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Intensity", &intensity, 0.1f);
}

void DirectionalLight::use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const
{
	Light::use(prefix, shader, flash);
	auto[t, r, s] = decomposeTransformation(getGlobalTransformation());
	glm::vec3 direction = glm::mat3_cast(r) * glm::vec3{0.0f, 0.0f, -1.0f};
	direction = glm::normalize(direction);
	shader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4(direction, 0.0f)));
}

void DirectionalLight::drawUI()
{
	Transformed<Rotation>::drawUI();
	Light::drawUI();
}

void PointLight::use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const
{
	Light::use(prefix, shader, flash);
	auto[t, r, s] = decomposeTransformation(getGlobalTransformation());
	shader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4(t, 1.0f)));
}

void PointLight::drawUI()
{
	Transformed<Translation>::drawUI();
	Light::drawUI();
}

void SpotLight::setCutoff(float inner, float outer)
{
	this->innerCutoff = inner;
	this->outerCutoff = outer;
}

float SpotLight::getInnerCutoff() const
{
	return innerCutoff;
}

float SpotLight::getOuterCutoff() const
{
	return outerCutoff;
}

void SpotLight::use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const
{
	Light::use(prefix, shader, flash);
	shader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4{getPosition(), 1.0f}));
	shader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4{getDirection(), 0.0f}));
	shader.set(prefix + "innerCutoff", glm::cos(glm::radians(getInnerCutoff())));
	shader.set(prefix + "outerCutoff", glm::cos(glm::radians(getOuterCutoff())));
}

void SpotLight::drawUI()
{
	Transformed<Translation, Rotation>::drawUI();
	Light::drawUI();
	ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.1f);
	ImGui::DragFloat("Outer Cutoff", &outerCutoff, 0.1f);
}
