#pragma once
#include "ResourceManager.h"
#include "Scene.h"

class SceneManager : public detail::ResourceManagerBase<Scene>
{
public:
	static void initialize();
	static Scene* importGLTF(std::string_view const filename);
	static Scene* basic();
	static Scene* testShadowMapping();

};
