#pragma once
#include "Named.h"
#include "Node.h"
#include "Camera.h"
#include "Lights.h"

#include <vector>
#include <memory>
#include <forward_list>

class Prop;

class Scene
{
private:
	std::vector<std::unique_ptr<Node>> nodes;
	float idealSize = 2.0f;
	Camera camera;
	std::vector<DirectionalLight> directionalLights{[](){DirectionalLight light; light.setOrientation({-30, -30, 0}); return light; } ()};
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	glm::vec3 backgroundColor{0.0f, 0.015f, 0.015f};
	//7Cubemap* skybox = nullptr;

public:
	Name<Scene> name{"scene"};

public:
	Scene(std::vector<std::unique_ptr<Node>>&& nodes = {});

public:
	std::vector<DirectionalLight> const& getDirectionalLights() const;
	std::vector<PointLight> const& getPointLights() const;
	std::vector<SpotLight> const& getSpotLights() const;
	glm::vec3 const& getBackground() const;
	Camera& getCamera();
	Camera const& getCamera() const;
	std::forward_list<Prop const*> getActiveProps() const;
	void fitToIdealSize();
	void drawUI();

};