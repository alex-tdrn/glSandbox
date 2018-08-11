#pragma once
#include "Named.h"
#include "Node.h"
#include "Camera.h"

#include <vector>
#include <memory>
#include <forward_list>

class Prop;

class Scene
{
private:
	std::vector<std::unique_ptr<Node>> nodes;
	Camera camera;

public:
	Name<Scene> name{"scene"};

public:
	Scene(std::vector<std::unique_ptr<Node>>&& nodes = {});

public:
	Camera& getCamera();
	std::forward_list<Prop const*> getActiveProps() const;
	void drawUI();

};