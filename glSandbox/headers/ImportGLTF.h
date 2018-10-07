#pragma once
#include "Scene.h"
#include "Mesh.h"
#include "Texture.h"

#include <string_view>
#include <vector>
#include <memory>
struct Asset
{
	std::vector<std::unique_ptr<Scene>> scenes;
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::vector<std::unique_ptr<Texture>> textures;
};

Asset import(std::string_view const& filename);