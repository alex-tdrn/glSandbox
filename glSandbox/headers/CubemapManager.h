#pragma once
#include "ResourceManager.h"
#include "Cubemap.h"

class CubemapManager : public ResourceManager<Cubemap>
{
public:
	static void initialize();
	static Cubemap* skybox();
};
