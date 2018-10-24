#pragma once
#include "AutoName.h"

#include <glm/glm.hpp>
#include <optional>

class Texture;
class Shader;

class Material : public AutoName<Material>
{
public:
	enum class Map
	{
		none = -1,
		normal,
		occlusion,
		emissive,
		baseColor,
		metallicRoughness
	};
private:
	Texture* normalMap = nullptr;
	bool normalMappingEnabled = true;
	Texture* occlusionMap = nullptr;
	bool occlusionMappingEnabled = true;
	Texture* emissiveMap = nullptr;
	glm::vec3 emissiveFactor = glm::vec3{0.0f};

public:
	Material() = default;
	virtual ~Material() = default;

protected:
	std::string getNamePrefix() const override;

public:
	void setNormal(Texture* map);
	void setOcclusion(Texture* map);
	void setEmissive(Texture* map, std::optional<glm::vec3> factor);
	virtual void use(Shader* shader, Material::Map visualizeMap = Material::Map::none) const;
	virtual void drawUI();
};