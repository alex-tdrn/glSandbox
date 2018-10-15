#pragma once
#include "ResourceManager.h"
#include "Texture.h"

class TextureManager : public detail::ResourceManagerBase<Texture>
{
public:
	static void initialize();
	static Texture* debug();
};
