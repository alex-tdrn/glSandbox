#include "Material.h"
#include "TextureManager.h"
#include "UIUtilities.h"
#include "ShaderManager.h"

void Material::setNormal(Texture* map)
{
	normalMap = map;
}

void Material::setOcclusion(Texture* map)
{
	occlusionMap = map;
}

void Material::setEmissive(Texture* map, std::optional<glm::vec3> factor)
{
	emissiveMap = map;
	if(!factor)
		emissiveFactor = glm::vec3{0.0f};
	else
		emissiveFactor = *factor;
}

void Material::use(Shader* shader, Material::Map visualizeMap) const
{
	if(shader == ShaderManager::pbr())
	{
		shader->set("material.normalMapExists", normalMap != nullptr && normalMappingEnabled);
		if(normalMap && normalMappingEnabled)
		{
			shader->set("material.normalMap", static_cast<int>(Map::normal));
			normalMap->use(static_cast<int>(Map::normal));
		}
		shader->set("material.occlusionMapExists", occlusionMap != nullptr && occlusionMappingEnabled);
		if(occlusionMap && occlusionMappingEnabled)
		{
			shader->set("material.occlusionMap", static_cast<int>(Map::occlusion));
			occlusionMap->use(static_cast<int>(Map::occlusion));
		}
		shader->set("material.emissiveMapExists", emissiveMap != nullptr);
		if(emissiveMap)
		{
			shader->set("material.emissiveMap", static_cast<int>(Map::emissive));
			emissiveMap->use(static_cast<int>(Map::emissive));
		}
		shader->set("material.emissiveFactor", emissiveFactor);
	}
	else if(shader == ShaderManager::unlit())
	{
		Texture* map = nullptr;
		glm::vec3 color{1.0f};
		switch(visualizeMap)
		{
			case Map::normal:
				map = normalMap;
				break;
			case Map::occlusion:
				map = occlusionMap;
				break;
			case Map::emissive:
				map = emissiveMap;
				color = emissiveFactor;
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
}

void Material::drawUI()
{
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Normals");
	ImGui::Text("Map");
	ImGui::SameLine();
	normalMap = chooseFromCombo(normalMap, TextureManager::getAll(), true, "Normals");
	if(normalMap)
	{
		ImGui::SameLine();
		ImGui::Checkbox("###NormalMappingEnabled", &normalMappingEnabled);
		normalMap->drawUI();
		ImGui::Separator();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Occlusion");
	ImGui::Text("Map");
	ImGui::SameLine();
	occlusionMap = chooseFromCombo(occlusionMap, TextureManager::getAll(), true, "Occlusion");
	if(occlusionMap)
	{
		ImGui::SameLine();
		ImGui::Checkbox("###OcclusionMappingEnabled", &occlusionMappingEnabled);
		occlusionMap->drawUI();
		ImGui::Separator();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Emissive");
	ImGui::SameLine();
	ImGui::ColorEdit3("###FactorEmissive", &emissiveFactor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
	ImGui::Text("Map");
	ImGui::SameLine();
	emissiveMap = chooseFromCombo(emissiveMap, TextureManager::getAll(), true, "Emmissive");
	if(emissiveMap)
		emissiveMap->drawUI();
}