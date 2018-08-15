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
	friend class Node;

private:
	glm::vec3 backgroundColor{0.0f, 0.015f, 0.015f};
	std::vector<std::unique_ptr<Node>> rootNodes;
	float idealSize = 2.0f;
	mutable struct{
		bool dirty = true;
		std::vector<Node*> abstractNodes;
		std::vector<Prop*> props;
	}primaryCache, secondaryCache;

	Camera camera;
	std::vector<DirectionalLight> directionalLights{[](){DirectionalLight light; light.setOrientation({-30, -30, 0}); return light; } ()};
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	//7Cubemap* skybox = nullptr;

public:
	Name<Scene> name{"scene"};

public:
	Scene(std::vector<std::unique_ptr<Node>>&& rootNodes = {});

private:
	void updatePrimaryCache() const;
	void updateSecondaryCache() const;

public:
	template<typename T>
	std::vector<T*> const& getAll() const;
	template<typename T>
	std::vector<T*> const& getEnabled() const;
	std::vector<DirectionalLight> const& getDirectionalLights() const;
	std::vector<PointLight> const& getPointLights() const;
	std::vector<SpotLight> const& getSpotLights() const;
	glm::vec3 const& getBackground() const;
	Camera& getCamera();
	Camera const& getCamera() const;
	void fitToIdealSize();
	void drawUI();

};

template<typename T>
std::vector<T*> const& Scene::getAll() const
{
	if(primaryCache.dirty)
		updatePrimaryCache();
	if constexpr(std::is_same<T, Node>())
		return primaryCache.abstractNodes;
	if constexpr(std::is_same<T, Prop>())
		return primaryCache.props;
}

template<typename T>
std::vector<T*> const& Scene::getEnabled() const
{
	if(primaryCache.dirty || secondaryCache.dirty)
		updateSecondaryCache();
	if constexpr(std::is_same<T, Node>())
		return secondaryCache.abstractNodes;
	if constexpr(std::is_same<T, Prop>())
		return secondaryCache.props;
}