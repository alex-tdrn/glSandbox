#pragma once
#include "ResourceManager.h"
#include "Cubemap.h"

class CubemapManager : public ResourceManager<Cubemap>
{
public:
	static void initialize();
	static Cubemap* skybox();
	static Cubemap* winterForest();
	static Cubemap* studio_small_05_1k();
	static Cubemap* studio_small_05_2k();
	static Cubemap* studio_small_05_4k();
	static Cubemap* studio_small_05_8k();

};
