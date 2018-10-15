#include "CubemapManager.h"


void CubemapManager::initialize()
{
	skybox();
}

Cubemap* load(std::string&& name, std::array<Texture, 6> faces)
{
	auto cubemap = std::make_unique<Cubemap>(std::move(faces));
	cubemap->name.set(std::move(name));
	return CubemapManager::add(std::move(cubemap));
}

Cubemap* CubemapManager::skybox()
{
	static auto ret = load("skybox",{
			Texture{"cubemaps/skybox/right.jpg", false},
			Texture{"cubemaps/skybox/left.jpg", false},
			Texture{"cubemaps/skybox/top.jpg", false},
			Texture{"cubemaps/skybox/bottom.jpg", false},
			Texture{"cubemaps/skybox/front.jpg", false},
			Texture{"cubemaps/skybox/back.jpg", false}
	});
	return ret;
}

