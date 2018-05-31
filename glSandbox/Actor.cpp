#include "Actor.h"
#include <imgui.h>

Actor::Actor(Model const* model, std::string const name)
	:model(model), name(name)
{

}

void Actor::setModel(Model const* model)
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
	if(!model)
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
	if(!model || !outlined)
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
	if(ImGui::Checkbox("Outlined", &outlined))
		valueChanged = true;
	if(position.drawUI())
		valueChanged = true;
	if(scale.drawUI())
		valueChanged = true;
	return valueChanged;
}