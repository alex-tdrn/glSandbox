#pragma once
#include "TransformedNode.h"

#include <glm/glm.hpp>
class Shader;

class Light
{
private:
	glm::vec3 color{1.0f};
	float intensity = 1.0f;

public:
	Light() = default;
	virtual ~Light() = default;

public:
	void setColor(glm::vec3 color);
	glm::vec3 const& getColor() const;
	void setIntensity(float intensity);
	float getIntensity() const;
	void use(std::string const& prefix, Shader& shader, bool flash) const;
	virtual void drawUI();

};

class DirectionalLight final : public Light, public Transformed<Rotation>
{
protected:
	std::string getNamePrefix() const override;

public:
	void use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const;
	void drawUI() override;

};

class PointLight final : public Light, public Transformed<Translation>
{
protected:
	std::string getNamePrefix() const override;

public:
	void use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const;
	void drawUI() override;

};

class SpotLight final : public Light, public Transformed<Translation, Rotation>
{
private:
	float innerCutoff = 15.0f;
	float outerCutoff = 20.0f;
	
protected:
	std::string getNamePrefix() const override;

public:
	void setCutoff(float inner, float outer);
	float getInnerCutoff() const;
	float getOuterCutoff() const;
	void use(std::string const& prefix, glm::mat4 const& viewMatrix, Shader& shader, bool flash) const;
	void drawUI() override;

};
