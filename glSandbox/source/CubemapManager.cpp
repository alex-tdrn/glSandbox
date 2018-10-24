#include "CubemapManager.h"


void CubemapManager::initialize()
{
	skybox();
	studio_small_05_1k();
	studio_small_05_2k();
	studio_small_05_4k();
	studio_small_05_8k();
	wooden_lounge_1k();
	wooden_lounge_2k();
	wooden_lounge_4k();
	wooden_lounge_8k();
	fireplace_1k();
	shanghai_bund_1k();
}

Cubemap* load(std::string&& name, std::array<Texture, 6> faces)
{
	auto cubemap = std::make_unique<Cubemap>(std::move(faces));
	cubemap->name.set(std::move(name));
	return CubemapManager::add(std::move(cubemap));
}

Cubemap* load(std::string&& name, Texture map)
{
	auto cubemap = std::make_unique<Cubemap>(std::move(map));
	cubemap->name.set(std::move(name));
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

Cubemap* CubemapManager::studio_small_05_1k()
{
	static auto ret = load("studio_small_05_1k",
		Texture{"../../studio_small_05_1k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::studio_small_05_2k()
{
	static auto ret = load("studio_small_05_2k",
		Texture{"../../studio_small_05_2k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::studio_small_05_4k()
{
	static auto ret = load("studio_small_05_4k",
		Texture{"../../studio_small_05_4k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::studio_small_05_8k()
{
	static auto ret = load("studio_small_05_8k",
		Texture{"../../studio_small_05_8k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::wooden_lounge_1k()
{
	static auto ret = load("wooden_lounge_1k",
		Texture{"../../wooden_lounge_1k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::wooden_lounge_2k()
{
	static auto ret = load("wooden_lounge_2k",
		Texture{"../../wooden_lounge_2k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::wooden_lounge_4k()
{
	static auto ret = load("wooden_lounge_4k",
		Texture{"../../wooden_lounge_4k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::wooden_lounge_8k()
{
	static auto ret = load("wooden_lounge_8k",
		Texture{"../../wooden_lounge_8k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::fireplace_1k()
{
	static auto ret = load("fireplace_1k",
		Texture{"../../fireplace_1k.hdr", true}
	);
	return ret;
}

Cubemap* CubemapManager::shanghai_bund_1k()
{
	static auto ret = load("shanghai_bund_1k",
		Texture{"../../shanghai_bund_1k.hdr", true}
	);
	return ret;
}