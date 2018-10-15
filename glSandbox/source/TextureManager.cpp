#include "TextureManager.h"

void TextureManager::initialize()
{
	debug();
}

Texture* TextureManager::debug()
{
	static auto ret = add(std::make_unique<Texture>("textures/debug.png", false));
	return ret;

}
