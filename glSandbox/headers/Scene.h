#pragma once
#include "AutoName.h"
#include "Camera.h"
#include "Lights.h"
#include "Cubemap.h"
#include "Timestamp.h"

#include <vector>
#include <memory>

class Prop;
class Camera;
class Node;

class Scene : public AutoName<Scene>
{

private:
	bool useSkybox = false;
	glm::vec3 backgroundColor{0.0f, 0.015f, 0.015f};
	Cubemap* skybox = nullptr;
	std::unique_ptr<Node> root = std::make_unique<TransformedNode>();
	float idealSize = 4.0f;
	mutable struct{
		bool dirty = true;
		std::vector<TransformedNode*> transformedNodes;
		std::vector<Camera*> cameras;
		std::vector<Prop*> props;
		std::vector<DirectionalLight*> directionalLights;
		std::vector<PointLight*> pointLights;
		std::vector<SpotLight*> spotLights;
	}cache;
	Node* current = nullptr;

public:
	Scene();
	Scene(Scene const&) = delete;
	Scene(Scene&&);
	Scene(std::unique_ptr<Node>&& root);
	Scene& operator=(Scene const&) = delete;
	Scene& operator=(Scene&&) = delete;
	~Scene() = default;

private:
	void addDefaultNodes();
	void updateCache() const;

protected:
	std::string getNamePrefix() const override;

public:
	void cacheOutdated() const;
	Node* getRoot() const;
	Node* getCurrent() const;
	template<typename T>
	std::vector<T*> const& getAll() const;
	template<typename T>
	std::vector<T*> getAllEnabled() const;
	bool usesSkybox() const;
	glm::vec3 const& getBackground() const;
	Cubemap const* getSkyBox() const;
	
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
	else if constexpr(std::is_same<T, DirectionalLight>())
		return cache.directionalLights;
	else if constexpr(std::is_same<T, PointLight>())
		return cache.pointLights;
	else if constexpr(std::is_same<T, SpotLight>())
		return cache.spotLights;
	else if constexpr(std::is_same<T, Light>())
	{
		static std::vector<Light*> ret;
		ret.clear();
		ret.reserve(cache.directionalLights.size() + cache.pointLights.size() + cache.spotLights.size());
		std::copy(cache.directionalLights.begin(), cache.directionalLights.end(), std::back_inserter(ret));
		std::copy(cache.pointLights.begin(), cache.pointLights.end(), std::back_inserter(ret));
		std::copy(cache.spotLights.begin(), cache.spotLights.end(), std::back_inserter(ret));
		return ret;
	}
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
