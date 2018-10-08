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
	void setBaseColor(Texture* map, std::optional<glm::vec4> factor);
	void setMetallicRoughness(Texture* map, float metallicFactor, float roughnessFactor);
	void use(Shader& shader) const override;
	void drawUI() override;
};