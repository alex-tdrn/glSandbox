#include "MaterialPBRMetallicRoughness.h"
#include "Util.h"

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

void MaterialPBRMetallicRoughness::use(Shader & shader) const
{
	Material::use(shader);
	//TODO
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
	ImGui::ColorEdit4("###Factor", &baseColorFactor.x, ImGuiColorEditFlags_NoInputs);
	if(baseColorMap)
		baseColorMap->drawUI();
	ImGui::Separator();
	ImGui::Text("Metallic-Roughness");
	ImGui::SliderFloat("Metallic", &metallicFactor, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &roughnessFactor, 0.0f, 1.0f);
	if(metallicRoughnessMap)
		metallicRoughnessMap->drawUI();
	ImGui::EndChild();
}
