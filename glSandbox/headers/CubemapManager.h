#pragma once
#include "ResourceManager.h"
#include "Cubemap.h"

class CubemapManager : public detail::ResourceManagerBase<Cubemap>
{
public:
	static void initialize();
	static Cubemap* skybox();
};
