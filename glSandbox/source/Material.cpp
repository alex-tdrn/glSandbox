#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Util.h"
#include "Resources.h"

#include <imgui.h>

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

void Material::use(Shader* shader) const
{
	if(shader == ResourceManager<Shader>::pbr())
	{
		shader->set("material.emissiveMapExists", emissiveMap != nullptr);
		if(emissiveMap)
		{
			shader->set("material.emissiveMap", 3);
			emissiveMap->use(3);
		}
		shader->set("material.emissiveFactor", emissiveFactor);
	}
}

void Material::drawUI()
{
	if(normalMap)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Normals");
		normalMap->drawUI();
		ImGui::Separator();
	}
	if(occlusionMap)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Occlusion");
		occlusionMap->drawUI();
		ImGui::Separator();
	}
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Emissive");
	ImGui::SameLine();
	ImGui::ColorEdit3("###FactorEmissive", &emissiveFactor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
	if(emissiveMap)
		emissiveMap->drawUI();
}