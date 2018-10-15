#pragma once
#include "ResourceManager.h"
#include "Material.h"

class MaterialManager : public detail::ResourceManagerBase<Material>
{
public:
	static void initialize();
	static Material* basic();
};