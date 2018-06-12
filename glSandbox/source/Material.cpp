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

void Material::setMap(int mapType, std::optional<Texture> map)
{
	maps[mapType] = map;
}

std::string_view const Material::getName() const
{
	return name;
}

bool Material::isInitialized() const
{
	bool ret = true;
	for(auto& map : maps)
		if(map)
			ret = ret && map->isInitialized();

	return ret;
}

bool Material::contains(std::string_view const path)
{
	for(auto& map : maps)
		if(map && map->getPath() == path)
			return true;
	return false;
}

void Material::use(Shader shader) const
{

	shader.set("material.hasDiffuseMap", bool(maps[diffuse]));
	if(maps[diffuse])
	{
		maps[diffuse]->use();
		shader.set("material.diffuseMap", maps[diffuse]->getLocation());
		shader.set("material.diffuseMapOffset", maps[diffuse]->getUVOffset());
	}

	shader.set("material.hasSpecularMap", bool(maps[specular]));
	if(maps[specular])
	{
		maps[specular]->use();
		shader.set("material.specularMap", maps[specular]->getLocation());
		shader.set("material.specularMapOffset", maps[specular]->getUVOffset());
		shader.set("material.shininess", shininessValue);
	}

	shader.set("material.hasOpacityMap", bool(maps[opacity]));
	if(maps[opacity])
	{
		maps[opacity]->use();
		shader.set("material.opacityMap", maps[opacity]->getLocation());
		shader.set("material.opacityMapOffset", maps[opacity]->getUVOffset());
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
					if(maps[mapType]->drawUI())
						valueChanged = true;
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