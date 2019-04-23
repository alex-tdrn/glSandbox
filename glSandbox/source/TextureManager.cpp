#include "TextureManager.h"

void TextureManager::initialize()
{
	uvChecker1();
	uvChecker2();
	uvChecker3S();
	uvChecker3M();
	uvChecker3L();
	uvChecker3XL();
}

Texture* TextureManager::uvChecker1()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker1.png", false));
	return ret;
}

Texture* TextureManager::uvChecker2()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker2.png", false));
	return ret;
}

Texture* TextureManager::uvChecker3S()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker3S.png", false));
	return ret;
}

Texture* TextureManager::uvChecker3M()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker3M.png", false));
	return ret;
}

Texture* TextureManager::uvChecker3L()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker3L.png", false));
	return ret;
}

Texture* TextureManager::uvChecker3XL()
{
	static auto ret = add(std::make_unique<Texture>("textures/uvChecker3XL.png", false));
	return ret;
}