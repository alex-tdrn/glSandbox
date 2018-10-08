#include "MaterialPBRMetallicRoughness.h"
#include "Util.h"
#include "Shader.h"
#include "Resources.h"

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

void MaterialPBRMetallicRoughness::use(Shader& shader) const
{
	Material::use(shader);
	if(&shader == &res::shaders()[res::ShaderType::pbr])
	{
		shader.set("material.baseColorMapExists", baseColorMap != nullptr);
		if(baseColorMap)
		{
			shader.set("material.baseColorMap", 4);
			baseColorMap->use(4);
		}
		shader.set("material.baseColorFactor", baseColorFactor);

		shader.set("material.metallicRoughnessMapExists", metallicRoughnessMap != nullptr);
		if(metallicRoughnessMap)
		{
			shader.set("material.metallicRoughnessMap", 5);
			metallicRoughnessMap->use(5);
		}
		shader.set("material.metallicFactor", metallicFactor);
		shader.set("material.roughnessFactor", roughnessFactor);
	}
	else if(&shader == &res::shaders()[res::ShaderType::blinn_phong] ||
		&shader == &res::shaders()[res::ShaderType::phong] ||
		&shader == &res::shaders()[res::ShaderType::gouraud] ||
		&shader == &res::shaders()[res::ShaderType::flat])
	{
		shader.set("material.hasSpecularMap", false);
		shader.set("material.overrideSpecular", true);
		shader.set("material.overrideSpecularColor", glm::vec3(1.0f));
		shader.set("material.shininess", 256.0f);
		shader.set("material.hasOpacityMap", false);

		shader.set("material.hasDiffuseMap", baseColorMap != nullptr);
		shader.set("material.overrideDiffuse", false);
		if(baseColorMap)
		{
			shader.set("material.diffuseMap", 4);
			baseColorMap->use(4);
		}
		else
		{
			shader.set("material.overrideDiffuseColor", glm::vec3(baseColorFactor));
		}
	}
	else if(&shader == &res::shaders()[res::ShaderType::unlit])
	{
		Texture* map = baseColorMap;
		glm::vec3 color = baseColorFactor;

		shader.set("material.hasMap", map != nullptr);
		if(map)
		{
			shader.set("material.map", 1);
			map->use(1);
		}
		shader.set("material.color", color);
	}
	else
	{
		assert(false);
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
