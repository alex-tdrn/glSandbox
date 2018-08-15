#pragma once
#include "Named.h"
#include "Node.h"
#include "Camera.h"
#include "Lights.h"

#include <vector>
#include <memory>

class Prop;

class Scene
{

private:
	glm::vec3 backgroundColor{0.0f, 0.015f, 0.015f};
	std::unique_ptr<Node> root;
	float idealSize = 2.0f;
	mutable struct{
		bool dirty = true;
		std::vector<Node*> abstractNodes;
		std::vector<Prop*> props;
	}cache;

	Camera camera;
	std::vector<DirectionalLight> directionalLights{[](){DirectionalLight light; light.setOrientation({-30, -30, 0}); return light; } ()};
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	//7Cubemap* skybox = nullptr;

public:
	Name<Scene> name{"scene"};

public:
	Scene(std::unique_ptr<Node>&& root = {});

private:
	void updateCache() const;

public:
	void cacheOutdated() const;
	template<typename T>
	std::vector<T*> const& getAll() const;

	std::vector<DirectionalLight> const& getDirectionalLights() const;
	std::vector<PointLight> const& getPointLights() const;
	std::vector<SpotLight> const& getSpotLights() const;
	glm::vec3 const& getBackground() const;
	Camera& getCamera();
	Camera const& getCamera() const;
	void fitToIdealSize() const;
	void drawUI();

};

template<typename T>
std::vector<T*> const& Scene::getAll() const
{
	if(cache.dirty)
		updateCache();
	if constexpr(std::is_same<T, Node>())
		return cache.abstractNodes;
	if constexpr(std::is_same<T, Prop>())
		return cache.props;
}

