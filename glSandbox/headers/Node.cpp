#include "Node.h"

Node::Node(Node&& other)
	:children(std::move(other.children))
{
	for(auto& child : children)
		child->parent = this;
}

std::vector<std::unique_ptr<Node>> const& Node::getChildren() const
{
	return children;
}

glm::mat4 const& Node::getTransformation() const
{
	if(parent == nullptr)
		return transformation;
	else
		return transformation * parent->getTransformation();
}
