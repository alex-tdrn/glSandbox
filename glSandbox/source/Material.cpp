#include "Material.h"
#include "TextureManager.h"
#include "UIUtilities.h"
#include "ShaderManager.h"

std::string Material::getNamePrefix() const
{
	return "material";
}

void Material::setNormalMap(Texture* map)
{
	normalMap = map;
}

void Material::enableNormalMapping()
{
	normalMapping = true;
}

void Material::disableNormalMapping()
{
	normalMapping = false;
}

void Material::setOcclusionMap(Texture* map)
{
	occlusionMap = map;
}

void Material::enableOcclusionMapping()
{
	occlusionMapping = true;
}

void Material::disableOcclusionMapping()
{
	occlusionMapping = false;
}

void Material::setEmissiveMap(Texture* map)
{
	emissiveMap = map;
}

void Material::setEmissiveFactor(glm::vec3 factor)
{
	emissiveFactor = factor;
}


void Material::use(Shader* shader, Material::Map visualizeMap) const
{
	if(shader == ShaderManager::pbr())
	{
		shader->set("material.normalMapExists", normalMap != nullptr && normalMapping);
		if(normalMap && normalMapping)
		{
			shader->set("material.normalMap", static_cast<int>(Map::normal));
			normalMap->use(static_cast<int>(Map::normal));
		}
		shader->set("material.occlusionMapExists", occlusionMap != nullptr && occlusionMapping);
		if(occlusionMap && occlusionMapping)
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
	ImGui::ChooseFromCombo("Normals", normalMap, TextureManager::getAll(), true,
		&Material::setNormalMap, this);
	if(normalMap)
	{
		ImGui::SameLine();
		ImGui::Checkbox("###NormalMappingEnabled", normalMapping, 
			&Material::enableNormalMapping, &Material::disableNormalMapping, this);
		normalMap->drawUI();
		ImGui::Separator();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Occlusion");
	ImGui::Text("Map");
	ImGui::SameLine();
	ImGui::ChooseFromCombo("Occlusion", occlusionMap, TextureManager::getAll(), true,
		&Material::setOcclusionMap, this);
	if(occlusionMap)
	{
		ImGui::SameLine();
		ImGui::Checkbox("###OcclusionMappingEnabled", occlusionMapping, 
			&Material::enableOcclusionMapping, &Material::disableOcclusionMapping, this);
		occlusionMap->drawUI();
		ImGui::Separator();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Emissive");
	ImGui::SameLine();
	ImGui::ColorEdit3("###FactorEmissive", emissiveFactor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float, 
		&Material::setEmissiveFactor, this);
	ImGui::Text("Map");
	ImGui::SameLine();
	ImGui::ChooseFromCombo("Emmissive", emissiveMap, TextureManager::getAll(), true,
		&Material::setEmissiveMap, this);
	if(emissiveMap)
		emissiveMap->drawUI();
}