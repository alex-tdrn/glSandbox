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
	Name<Node> name{"node"};
	Scene* scene = nullptr;
	bool enabled = true;
	bool highlighted = false;
	std::vector<std::unique_ptr<Node>> children;

protected:
	Node* parent = nullptr;
	glm::vec3 localTranslation{0.0f};
	glm::vec3 localRotation{0.0f};
	glm::vec3 localScale{1.0f};

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
	Scene& getScene() const;
	bool isEnabled() const;
	bool isHighlighted() const;
	void setHighlighted(bool);
	void enable();
	void disable();
	void addChild(std::unique_ptr<Node>&& node, bool retainGlobalTransformation = false);
	void addChildren(std::vector<std::unique_ptr<Node>>&& nodes, bool retainGlobalTransformation = false);
	std::unique_ptr<Node> release();
	std::vector<std::unique_ptr<Node>> releaseChildren();
	std::vector<std::unique_ptr<Node>> const& getChildren() const;
	virtual std::string const& getName() const;
	template<typename Callable>
	void recursive(Callable operation);
	void setLocalTransformation(glm::mat4&& t);
	void setLocalTransformation(glm::vec3&& t, glm::vec3&& r, glm::vec3&& s);
	virtual glm::mat4 getLocalTransformation() const;
	virtual glm::mat4 getGlobalTransformation() const;
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
