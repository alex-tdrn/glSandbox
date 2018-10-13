#pragma once
#include "Named.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <type_traits>
class Scene;

class Node
{
	friend class Scene;
	friend class std::unique_ptr<Node>;

private:
	Scene* scene = nullptr;
	bool enabled = true;
	bool highlighted = false;
	std::vector<std::unique_ptr<Node>> children;

protected:
	Node* parent = nullptr;

public:
	Name<Node> name{"node"};

public:
	Node() = default;
	Node(Node const&) = delete;
	Node(Node&&);
	Node(std::vector<std::unique_ptr<Node>>&&);
	virtual ~Node() = default;
	Node& operator=(Node const&) = delete;
	Node& operator=(Node&&) = delete;

private:
	void invalidateSceneCache();
	void setScene(Scene* scene);
	std::unique_ptr<Node> releaseChild(Node* node);

public:
	Scene* getScene() const;
	bool isEnabled() const;
	void enable();
	void disable();
	bool isHighlighted() const;
	void setHighlighted(bool);
	Node* addChild(std::unique_ptr<Node>&& node, bool retainGlobalTransformation = false);
	void addChildren(std::vector<std::unique_ptr<Node>>&& nodes, bool retainGlobalTransformation = false);
	std::unique_ptr<Node> release();
	std::vector<std::unique_ptr<Node>> releaseChildren();
	std::vector<std::unique_ptr<Node>> const& getChildren() const;
	virtual void setLocalTransformation(glm::mat4&&) = 0;
	virtual glm::mat4 getLocalTransformation() const = 0;
	virtual glm::mat4 getGlobalTransformation() const = 0;
	virtual Bounds getBounds() const;
	virtual void drawUI();
	template<typename Callable>
	void recursive(Callable operation);

};

template<typename Callable>
void Node::recursive(Callable operation)
{
	for(auto const& child : children)
		child.get()->recursive(operation);
	if constexpr(std::is_member_function_pointer_v<Callable>)
		(this->*operation)();
	else
		operation(this);
}
