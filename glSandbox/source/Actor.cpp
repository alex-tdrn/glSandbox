#include "Actor.h"
#include "GLobals.h"

#include <imgui.h>

Actor::Actor(Model* model, std::string name)
	:model(model), name(std::move(name))
{

}

void Actor::enable()
{
	enabled = true;
}

void Actor::disable()
{
	enabled = false;
}

void Actor::setModel(Model* model)
{
	this->model = model;
}

void Actor::setPosition(glm::vec3 position)
{
	this->position.position = position;
}

void Actor::setScale(glm::vec3 scale)
{
	this->scale.scale = scale;
}

void Actor::draw(Shader shader) const
{
	if(!enabled || !model)
		return;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position.position);
	modelMatrix = glm::scale(modelMatrix, scale.scale);
	if(!outlined)
	{
		glStencilMask(0x00);
	}
	else
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
	shader.set("model", modelMatrix);
	model->draw(shader);
}

void Actor::drawOutline(Shader outlineShader) const
{
	if(!enabled || !model || !outlined)
		return;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position.position);
	modelMatrix = glm::scale(modelMatrix, scale.scale);
	outlineShader.set("model", modelMatrix);
	
	model->draw(outlineShader);

}
bool Actor::drawUI()
{
	bool valueChanged = false;
	std::string_view comboPreview = "None";
	if(model)
		comboPreview = model->getName();
	if(ImGui::BeginCombo("Model", comboPreview.data()))
	{
		if(ImGui::Selectable("None", !model))
		{
			model = nullptr;
			resources::scene.update();
		}
		if(ImGui::Selectable(resources::models::nanosuit.getName().data(), model == &resources::models::nanosuit))
		{
			model = &resources::models::nanosuit;
			resources::scene.update();
		}
		if(ImGui::Selectable(resources::models::sponza.getName().data(), model == &resources::models::sponza))
		{
			model = &resources::models::sponza;
			resources::scene.update();
		}
		ImGui::EndCombo();
	}
	if(ImGui::Checkbox("Enabled", &enabled))
		valueChanged = true;
	ImGui::SameLine();
	if(ImGui::Checkbox("Outlined", &outlined))
		valueChanged = true;
	if(position.drawUI())
		valueChanged = true;
	if(scale.drawUI())
		valueChanged = true;
	return valueChanged;
}