#include "CubemapManager.h"


void CubemapManager::initialize()
{
	skybox();
}

Cubemap* CubemapManager::skybox()
{
	static auto ret = [&]() -> Cubemap*{
		std::array<Texture, 6> faces = {
			Texture{"cubemaps/skybox/right.jpg", false},
			Texture{"cubemaps/skybox/left.jpg", false},
			Texture{"cubemaps/skybox/top.jpg", false},
			Texture{"cubemaps/skybox/bottom.jpg", false},
			Texture{"cubemaps/skybox/front.jpg", false},
			Texture{"cubemaps/skybox/back.jpg", false}
		};
		auto cubemap = std::make_unique<Cubemap>(std::move(faces));
		cubemap->name.set("skybox");
		return add(std::move(cubemap));
	}();
	return ret;

}

