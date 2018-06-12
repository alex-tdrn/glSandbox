#pragma once
#include "Components.h"
#include <glm/glm.hpp>

class Light
{
private:
	bool enabled = true;
	glm::vec3 color{1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;

public:
	bool isEnabled() const;
	void enable();
	void disable();
	void setColor(glm::vec3 color);
	glm::vec3 const& getColor() const;
	void setIntensity(float intensity);
	float getIntensity() const;
	[[nodiscard]]virtual bool drawUI();

};
class DirectionalLight : public Light
{
private:
	Orientation orientation{120.0f, -60.0f, 0.0f};

public:
	void setOrientation(glm::vec3 orientation);
	glm::vec3 const getDirection() const;
	bool drawUI() override;

};

class PointLight : public Light
{
private:
	Position position = {{-1.0f, 0.0f, 2.0f}};

public:
	void setPosition(glm::vec3 position);
	glm::vec3 const& getPosition() const;
	bool drawUI() override;

};

class SpotLight : public Light
{
private:
	Position position = {{1.0f, 0.0f, 3.0f}};
	Orientation orientation;
	float innerCutoff = 15.0f;
	float outerCutoff = 20.0f;

public:
	void setPosition(glm::vec3 position);
	void setOrientation(glm::vec3 orientation);
	glm::vec3 const& getPosition() const;
	glm::vec3 const getDirection() const;
	float getInnerCutoff() const;
	float getOuterCutoff() const;
	bool drawUI() override;

};
