#pragma once
#include "Named.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Node
{
private:
	Node* parent = nullptr;
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
public:
	std::vector<std::unique_ptr<Node>> const& getChildren() const;
	void addChild(std::unique_ptr<Node>&& node);
	glm::mat4 getTransformation() const;
	virtual void drawUI();

};