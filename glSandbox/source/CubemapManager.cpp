#include "CubemapManager.h"


void CubemapManager::initialize()
{
	skybox();
	brick_factory_01_4k();
	abandoned_factory_canteen_01_4k();
	winter_evening_4k();
}

Cubemap* load(std::string&& name, std::array<Texture, 6> faces)
{
	auto cubemap = std::make_unique<Cubemap>(std::move(faces));
	cubemap->setName(std::move(name));
	return CubemapManager::add(std::move(cubemap));
}

Cubemap* load(std::string&& name, Texture map)
{
	auto cubemap = std::make_unique<Cubemap>(std::move(map));
	cubemap->setName(std::move(name));
	return CubemapManager::add(std::move(cubemap));
}

Cubemap* CubemapManager::skybox()
{
	static auto ret = load("skybox", {
		Texture{"cubemaps/skybox/right.jpg", false},
		Texture{"cubemaps/skybox/left.jpg", false},
		Texture{"cubemaps/skybox/top.jpg", false},
		Texture{"cubemaps/skybox/bottom.jpg", false},
		Texture{"cubemaps/skybox/front.jpg", false},
		Texture{"cubemaps/skybox/back.jpg", false}
		});
	return ret;
}

Cubemap* CubemapManager::brick_factory_01_4k()
{
	static auto ret = load("brick_factory_01_4k",
		Texture{"cubemaps/brick_factory_01_4k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::abandoned_factory_canteen_01_4k()
{
	static auto ret = load("abandoned_factory_canteen_01_4k",
		Texture{"cubemaps/abandoned_factory_canteen_01_4k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::winter_evening_4k()
{
	static auto ret = load("winter_evening_4k",
		Texture{"cubemaps/winter_evening_4k.hdr", true}
	);
	return ret;
}

