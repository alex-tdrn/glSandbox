#include "MaterialPBRMetallicRoughness.h"
#include "UIUtilities.h"
#include "ShaderManager.h"
#include "TextureManager.h"

void MaterialPBRMetallicRoughness::setBaseColor(Texture* map, std::optional<glm::vec4> factor)
{
	baseColorMap = map;
	if(!factor)
		baseColorFactor = glm::vec4{1.0f};
	else
		baseColorFactor = *factor;
}

void MaterialPBRMetallicRoughness::setMetallicRoughness(Texture* map, float metallicFactor, float roughnessFactor)
{
	metallicRoughnessMap = map;
	this->metallicFactor = metallicFactor;
	this->roughnessFactor = roughnessFactor;
}

void MaterialPBRMetallicRoughness::use(Shader* shader, Material::Map visualizeMap) const
{
	Material::use(shader, visualizeMap);
	if(shader == ShaderManager::pbr())
	{
		shader->set("material.baseColorMapExists", baseColorMap != nullptr);
		if(baseColorMap)
		{
			shader->set("material.baseColorMap", static_cast<int>(Map::baseColor));
			baseColorMap->use(static_cast<int>(Map::baseColor));
		}
		shader->set("material.baseColorFactor", baseColorFactor);

		shader->set("material.metallicRoughnessMapExists", metallicRoughnessMap != nullptr);
		if(metallicRoughnessMap)
		{
			shader->set("material.metallicRoughnessMap", static_cast<int>(Map::metallicRoughness));
			metallicRoughnessMap->use(static_cast<int>(Map::metallicRoughness));
		}
		shader->set("material.metallicFactor", metallicFactor);
		shader->set("material.roughnessFactor", roughnessFactor);
	}
	else if(ShaderManager::isLightingShader(shader))
	{
		shader->set("material.hasSpecularMap", false);
		shader->set("material.overrideSpecular", true);
		shader->set("material.overrideSpecularColor", glm::vec3(1.0f));
		shader->set("material.shininess", 256.0f);
		shader->set("material.hasOpacityMap", false);

		shader->set("material.hasDiffuseMap", baseColorMap != nullptr);
		shader->set("material.overrideDiffuse", false);
		if(baseColorMap)
		{
			shader->set("material.diffuseMap", static_cast<int>(Map::baseColor));
			baseColorMap->use(static_cast<int>(Map::baseColor));
		}
		else
		{
			shader->set("material.overrideDiffuseColor", glm::vec3(baseColorFactor));
		}
	}
	else if(shader == ShaderManager::unlit())
	{
		Texture* map = nullptr;
		glm::vec3 color{1.0f};
		switch(visualizeMap)
		{
			case Map::baseColor:
				map = baseColorMap;
				color = baseColorFactor;
				break;
			case Map::metallicRoughness:
				map = metallicRoughnessMap;
				color = glm::vec3(metallicFactor, roughnessFactor, 0.0f);
				break;
			default:
				return;
		}
		shader->set("material.hasMap", map != nullptr);
		if(map)
		{
			shader->set("material.linear", map->isLinear());
			shader->set("material.map", 1);
			map->use(1);
		}
		shader->set("material.color", color);
	}
	else
	{
		//assert(false);
	}
}

void MaterialPBRMetallicRoughness::drawUI()
{
	IDGuard idGuard{this};
	ImGui::BeginChild("###Material");

	Material::drawUI();
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Base Color");
	ImGui::SameLine();
	ImGui::ColorEdit4("###FactorBaseColor", &baseColorFactor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
	ImGui::Text("Map");
	ImGui::SameLine();
	baseColorMap = chooseFromCombo(baseColorMap, TextureManager::getAll(), true, "BaseColor");
	if(baseColorMap)
		baseColorMap->drawUI();
	ImGui::Separator();
	ImGui::Text("Metallic-Roughness");
	ImGui::SliderFloat("Metallic", &metallicFactor, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &roughnessFactor, 0.0f, 1.0f);
	ImGui::Text("Map");
	ImGui::SameLine();
	metallicRoughnessMap = chooseFromCombo(metallicRoughnessMap, TextureManager::getAll(), true, "MetallicRoughness");
	if(metallicRoughnessMap)
		metallicRoughnessMap->drawUI();
	ImGui::EndChild();
}
