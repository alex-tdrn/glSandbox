#include "Material.h"
#include "Texture.h"
#include <imgui.h>

int Material::ct = 0;

Material::Material(std::optional<Texture> diffuseMap, std::optional<Texture> specularMap, float shininess, std::string const name)
	: name(name), diffuseMap(diffuseMap), specularMap(specularMap), shininess(shininess)
{
}
std::string_view const Material::getName() const
{
	return name;
}

bool Material::isInitialized() const
{
	bool ret = true;
	if(diffuseMap)
		ret = ret && diffuseMap->isInitialized();
	if(specularMap)
		ret = ret && specularMap->isInitialized();
	return ret;
}

bool Material::contains(std::string_view const path)
{
	for(auto& map : {diffuseMap, specularMap})
		if(map && map->getPath() == path)
			return true;
	return false;
}

void Material::use(Shader shader) const
{
	shader.set("material.hasDiffuseMap", bool(diffuseMap));
	if(diffuseMap)
	{
		diffuseMap->use(GL_TEXTURE0);
		shader.set("material.diffuse", 0);
	}

	shader.set("material.hasSpecularMap", bool(specularMap));
	if(specularMap)
	{
		specularMap->use(GL_TEXTURE1);
		shader.set("material.specular", 1);
		shader.set("material.shininess", shininess);
	}
}

void Material::drawUI()
{
	std::string header = name;
	if(!isInitialized())
		header += "(loading...)";
	if(ImGui::TreeNode(header.data()))
	{
		if(specularMap)
			ImGui::DragFloat("Shininess", &shininess, 0.1f);
		ImGui::Columns(2, nullptr, false);
		ImGui::Text("Diffuse Map");
		if(diffuseMap)
			diffuseMap->drawUI();
		else
			ImGui::Text("Missing");
		ImGui::NextColumn();
		ImGui::Text("Specular Map");
		if(specularMap)
			specularMap->drawUI();
		else
			ImGui::Text("Missing");
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::TreePop();
	}
}