#pragma once
#include "ResourceManager.h"
#include "Cubemap.h"

class CubemapManager : public ResourceManager<Cubemap>
{
public:
	static void initialize();
	static Cubemap* skybox();
	static Cubemap* brick_factory_01_4k();
	static Cubemap* abandoned_factory_canteen_01_4k();
	static Cubemap* winter_evening_4k();
};
