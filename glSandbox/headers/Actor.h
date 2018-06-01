#pragma once

#include "Model.h"
#include "Components.h"

class Actor
{
private:
	bool enabled = true;
	bool outlined = false;
	Position position;
	Scale scale;

	Model* model = nullptr;
	std::string name;
	inline static int ct = 0;

public:
	Actor(Model* model = nullptr, std::string name = "Actor#" + std::to_string(ct++));
	Actor& operator=(Actor const&) = default;

public:
	void enable();
	void disable();
	void setModel(Model* model);
	void setPosition(glm::vec3 position);
	void setScale(glm::vec3 scale);
	void draw(Shader shader) const;
	void drawOutline(Shader outlineShader) const;
	[[nodiscard]]bool drawUI();
};