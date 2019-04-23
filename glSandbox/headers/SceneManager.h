#pragma once
#include "ResourceManager.h"
#include "Scene.h"

class SceneManager : public ResourceManager<Scene>
{
public:
	static void initialize();
	static Scene* importGLTF(std::string_view const filename);
	static Scene* basic();
	static Scene* testShadowMapping();

};
