#pragma once
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

public:
	void setOrientation(glm::vec3 orientation);
	glm::vec3 const getDirection() const;
	bool drawUI() override;

};

class PointLight : public Light
{

public:
	void setPosition(glm::vec3 position);
	glm::vec3 const& getPosition() const;
	bool drawUI() override;

};

class SpotLight : public Light
{
	float innerCutoff = 15.0f;
	float outerCutoff = 20.0f;

public:
	void setPosition(glm::vec3 position);
	void setOrientation(glm::vec3 orientation);
	void setCutoff(float inner, float outer);
	glm::vec3 const& getPosition() const;
	glm::vec3 const getDirection() const;
	float getInnerCutoff() const;
	float getOuterCutoff() const;
	bool drawUI() override;

};
