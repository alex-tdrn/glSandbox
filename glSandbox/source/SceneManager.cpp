#include "SceneManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "ImportGLTF.h"
#include "Prop.h"

void SceneManager::initialize()
{
	basic();
	testShadowMapping();
}

Scene* SceneManager::importGLTF(std::string_view const filename)
{
	auto asset = import(filename);
	auto ret = asset.scenes[0].get();//active scene
	add(std::move(asset.scenes));
	MeshManager::add(std::move(asset.meshes));
	TextureManager::add(std::move(asset.textures));
	MaterialManager::add(std::move(asset.materials));
	return ret;
}

Scene* SceneManager::basic()
{
	static auto ret = [&]() -> Scene*{
		auto scene = std::make_unique<Scene>();
		scene->setName("basic");
		auto prop = std::make_unique<Prop>(MeshManager::box());
		prop->setLocalRotation({45.0f, 45.0f, 0.0f});
		prop->setLocalTranslation({0.0f, 1.0f, 0.0f});
		scene->getRoot()->addChild(std::move(prop));
		return add(std::move(scene));
	}();
	return ret;
}

Scene* SceneManager::testShadowMapping()
{
	static auto ret = [&]() -> Scene*{
		auto scene = std::make_unique<Scene>();
		scene->setName("testShadowMapping");
		auto getRandomFloat = [](float min, float max) -> float{
			int const precision = 1'000;
			float r = rand() % precision;
			r /= precision;
			float range = std::abs(max - min);
			return min + r * range;
		};
		auto getRandomVector = [&](float min, float max)->glm::vec3{
			return {
				getRandomFloat(min, max),
				getRandomFloat(min, max),
				getRandomFloat(min, max)
			};
		};
		const int gridSize = 8;
		const float stepSize = 3.0f;
		for(int x = -gridSize / 2; x <= gridSize / 2; x++)
		{
			for(int z = -gridSize / 2; z <= gridSize / 2; z++)
			{
				if(x == 0 && z == 0)
					continue;
				auto prop = std::make_unique<Prop>(MeshManager::box());
				prop->setLocalRotation(getRandomVector(-90.0f, 90.0f));
				glm::vec3 pos{x * stepSize, getRandomFloat(0.0f, 10.0f), z * stepSize};
				pos += getRandomVector(-stepSize / 10, stepSize / 10);
				prop->setLocalTranslation(pos);
				scene->getRoot()->addChild(std::move(prop));

			}
		}
		auto light = std::make_unique<PointLight>();
		light->setIntensity(10'000.0f);
		light->setColor({1.0f, 0.8f, 0.5f});
		scene->getRoot()->addChild(std::move(light));
		return add(std::move(scene));
	}();
	return ret;
}
