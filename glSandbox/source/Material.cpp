#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Util.h"
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

	//shader.set("material.hasDiffuseMap", bool(maps[diffuse]));
	//if(maps[diffuse])
	//{
	//	maps[diffuse]->use(diffuse);
	//	shader.set("material.diffuseMap", diffuse);
	//}

	//shader.set("material.hasSpecularMap", bool(maps[specular]));
	//if(maps[specular])
	//{
	//	maps[specular]->use(specular);
	//	shader.set("material.specularMap", specular);
	//	shader.set("material.shininess", shininessValue);
	//}

	//shader.set("material.hasOpacityMap", bool(maps[opacity]));
	//if(maps[opacity])
	//{
	//	maps[opacity]->use(opacity);
	//	shader.set("material.opacityMap", opacity);
	//}

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
	ImGui::ColorEdit3("###Factor", &emissiveFactor.x, ImGuiColorEditFlags_NoInputs);
	if(emissiveMap)
		emissiveMap->drawUI();
}