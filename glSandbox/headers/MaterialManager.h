#pragma once
#include "ResourceManager.h"
#include "Material.h"

class MaterialManager : public ResourceManager<Material>
{
public:
	static void initialize();
	static Material* uvChecker();
};