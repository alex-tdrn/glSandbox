#pragma once
#include "Material.h"

class MaterialPBRMetallicRoughness : public Material
{
private:
	Texture* baseColorMap = nullptr;
	glm::vec4 baseColorFactor = glm::vec4{1.0f};
	Texture* metallicRoughnessMap = nullptr;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;

public:
	void setBaseColorMap(Texture* map);
	void setBaseColorFactor(glm::vec4 factor);
	void setMetallicRoughnessMap(Texture* map);
	void setMetallicFactor(float factor);
	void setRoughnessFactor(float factor);
	void use(Shader* shader, Material::Map visualizeMap = Material::Map::none) const override;
	void drawUI() override;
};