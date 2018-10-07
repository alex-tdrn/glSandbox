#include "Material.h"
#include "Texture.h"
#include "Util.h"
#include <imgui.h>

std::string mapToString(int mapType)
{
	switch(mapType)
	{
		case Material::Maps::ambient:
			return "Ambient";
		case Material::Maps::diffuse:
			return "Diffuse";
		case Material::Maps::specular:
			return "Specular";
		case Material::Maps::shininess:
			return "Shininess";
		case Material::Maps::emission:
			return "Emission";
		case Material::Maps::light:
			return "Light";
		case Material::Maps::reflection:
			return "Reflection";
		case Material::Maps::opacity:
			return "Opacity";
		case Material::Maps::normal:
			return "Normal";
		case Material::Maps::bump:
			return "Bump";
		case Material::Maps::displacement:
			return "Displacement";
		default:
			return "Unknown";
	}
}

Material::Material(std::string const name)
	: name(name)
{
}

std::string Material::mapTypeToString(Material::Maps mapType)
{
	switch(mapType)
	{
		case ambient:
			return "Ambient";
		case diffuse:
			return "Diffuse";
		case specular:
			return "Specular";
		case shininess:
			return "Shininess";
		case emission:
			return "Emission";
		case light:
			return "Light";
		case reflection:
			return "Reflection";
		case opacity:
			return "Opacity";
		case normal:
			return "Normal";
		case bump:
			return "Bump";
		case displacement:
			return "Displacement";
	}
	return "Unknown Map Type";
}

void Material::setMap(int mapType, std::optional<Texture>&& map)
{
	maps[mapType] = std::move(map);
}

std::string_view const Material::getName() const
{
	return name;
}

bool Material::isInitialized() const
{
	bool ret = true;
	return true;
	//for(auto& map : maps)
	//	if(map)
	//		ret = ret && map->isLoaded();

	return ret;
}

bool Material::contains(std::string_view const path)
{
	//for(auto& map : maps)
	//	if(map && map->getPath() == path)
	//		return true;
	return false;
}

void Material::use(Shader shader) const
{

	shader.set("material.hasDiffuseMap", bool(maps[diffuse]));
	if(maps[diffuse])
	{
		maps[diffuse]->use(diffuse);
		shader.set("material.diffuseMap", diffuse);
	}

	shader.set("material.hasSpecularMap", bool(maps[specular]));
	if(maps[specular])
	{
		maps[specular]->use(specular);
		shader.set("material.specularMap", specular);
		shader.set("material.shininess", shininessValue);
	}

	shader.set("material.hasOpacityMap", bool(maps[opacity]));
	if(maps[opacity])
	{
		maps[opacity]->use(opacity);
		shader.set("material.opacityMap", opacity);
	}

}

bool Material::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	std::string header = name;
	if(!isInitialized())
		header += "(loading...)";
	if(ImGui::TreeNode(header.data()))
	{
		if(maps[specular])
			ImGui::DragFloat("Shininess", &shininessValue, 0.1f);

		int mapType = 0;
		int columnsEmpty = 0;
		while(mapType < Maps::n)
		{
			if(maps[mapType])
			{
				ImGui::Columns(2, nullptr, false);
				columnsEmpty = 2;
			}
			else
				mapType++;
			while(mapType < Maps::n && columnsEmpty > 0)
			{
				if(maps[mapType])
				{
					ImGui::Text((mapToString(mapType) + " Map").c_str());
					//valueChanged |= maps[mapType]->drawUI();
					ImGui::NextColumn();
					columnsEmpty--;
				}
				mapType++;
			}
		}
		while(columnsEmpty)
		{
			ImGui::NextColumn();
			columnsEmpty--;
		}
		ImGui::Columns(1, nullptr, false);
		ImGui::TreePop();
	}
	return valueChanged;
}