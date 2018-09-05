#pragma once
#include "Named.h"
#include "Camera.h"
#include "Lights.h"

#include <vector>
#include <memory>

class Prop;
class Camera;
class Node;

class Scene
{

private:
	glm::vec3 backgroundColor{0.0f, 0.015f, 0.015f};
	std::unique_ptr<Node> root = std::make_unique<TransformedNode>();
	float idealSize = 2.0f;
	mutable struct{
		bool dirty = true;
		std::vector<TransformedNode*> transformedNodes;
		std::vector<Camera*> cameras;
		std::vector<Prop*> props;
	}cache;

	std::vector<DirectionalLight> directionalLights{[](){DirectionalLight light; light.setOrientation({-30, -30, 0}); return light; } ()};
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	//7Cubemap* skybox = nullptr;

public:
	Name<Scene> name{"scene"};

public:
	Scene();
	Scene(Scene const&) = delete;
	Scene(Scene&&);
	Scene(std::unique_ptr<Node>&& root);
	Scene& operator=(Scene const&) = delete;
	Scene& operator=(Scene&&) = delete;
	~Scene() = default;

private:
	void updateCache() const;

public:
	void cacheOutdated() const;
	Node* getRoot() const;
	template<typename T>
	std::vector<T*> const& getAll() const;
	template<typename T>
	std::vector<T*> getAllEnabled() const;
	std::vector<DirectionalLight> const& getDirectionalLights() const;
	std::vector<PointLight> const& getPointLights() const;
	std::vector<SpotLight> const& getSpotLights() const;
	glm::vec3 const& getBackground() const;
	void fitToIdealSize() const;
	void drawUI();

};

template<typename T>
std::vector<T*> const& Scene::getAll() const
{
	if(cache.dirty)
		updateCache();
	if constexpr(std::is_same<T, TransformedNode>())
		return cache.transformedNodes;
	else if constexpr(std::is_same<T, Camera>())
		return cache.cameras;
	else if constexpr(std::is_same<T, Prop>())
		return cache.props;
}

template<typename T>
std::vector<T*> Scene::getAllEnabled() const
{
	std::vector<T*> ret;
	for(auto n : getAll<T>())
		if(n->isEnabled())
			ret.push_back(n);
	return ret;
}
