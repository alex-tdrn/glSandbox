#pragma once
#include "Named.h"
#include "Util.h"

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
	Node* parent = nullptr;
	std::vector<std::unique_ptr<Node>> children;
	glm::mat4 transformation{1.0f};

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
	void addChild(std::unique_ptr<Node>&& node);
	void addChildren(std::vector<std::unique_ptr<Node>>&& nodes);
	std::unique_ptr<Node> release();
	std::vector<std::unique_ptr<Node>> const& getChildren() const;
	template<typename Callable>
	void recursive(Callable operation);
	void setTransformation(glm::mat4&& t);
	glm::mat4 getTransformation() const;
	virtual Bounds getBounds() const;
	virtual void drawUI();

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
