#pragma once
#include "Named.h"

#include <glm/glm.hpp>
#include <optional>

class Texture;
class Shader;

class Material
{
private:
	Texture* normalMap = nullptr;
	bool normalMappingEnabled = true;
	Texture* occlusionMap = nullptr;
	bool occlusionMappingEnabled = true;
	Texture* emissiveMap = nullptr;
	glm::vec3 emissiveFactor = glm::vec3{0.0f};

public:
	Name<Material> name{"material"};

public:
	Material() = default;
	virtual ~Material() = default;

public:
	void setNormal(Texture* map);
	void setOcclusion(Texture* map);
	void setEmissive(Texture* map, std::optional<glm::vec3> factor);
	virtual void use(Shader* shader) const;
	virtual void drawUI();
};