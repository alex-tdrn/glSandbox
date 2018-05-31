#pragma once

#include "Model.h"
#include "Components.h"

class Actor
{
private:
	bool outlined = false;
	Position position;
	Scale scale;

	Model const* model = nullptr;
	std::string const name;
	inline static int ct = 0;

public:
	Actor(Model const* model = nullptr, std::string const name = "Actor#" + std::to_string(ct++));

public:
	void setModel(Model const* model);
	void setPosition(glm::vec3 position);
	void setScale(glm::vec3 scale);
	void draw(Shader shader) const;
	void drawOutline(Shader outlineShader) const;
	bool drawUI();
};