#pragma once
#include "Named.h"
#include "Util.h"

#include <vector>
#include <memory>

class Scene;

class Node
{
	friend class Scene;

private:
	Scene* scene = nullptr;
	Node* parent = nullptr;
	bool enabled = true;
	std::vector<std::unique_ptr<Node>> children;
	glm::mat4 transformation{1.0f};

public:
	Name<Node> name{"node"};

public:
	Node() = default;
	Node(Node const&) = delete;
	Node(Node&&);
	virtual ~Node() = default;
	Node& operator=(Node const&) = delete;
	Node& operator=(Node&&) = delete;

private:
	void invalidateSceneCache(int id);
	void setScene(Scene* scene);
	void enable();
	void disable();

public:
	std::vector<std::unique_ptr<Node>> const& getChildren() const;
	void addChild(std::unique_ptr<Node>&& node);
	void setTransformation(glm::mat4&& t);
	glm::mat4 getTransformation() const;
	virtual Bounds getBounds() const;
	virtual void drawUI();

};