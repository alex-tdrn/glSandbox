#include "Node.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

Node::Node(Node&& other)
	:parent(other.parent), scene(other.scene)
{
	addChildren(std::move(other.children));
}

Node::Node(std::vector<std::unique_ptr<Node>>&& nodes)
{
	addChildren(std::move(nodes));
}

void Node::invalidateSceneCache()
{
	if(scene)
		scene->cacheOutdated();
}

void Node::setScene(Scene * scene)
{
	this->scene = scene;
	for(auto const& child : children)
		child->setScene(scene);
}

std::unique_ptr<Node> Node::releaseChild(Node * node)
{
	invalidateSceneCache();
	for(auto it = children.begin(); it != children.end(); it++)
	{
		if(it->get() == node)
		{
			std::unique_ptr<Node> ret = std::move(*it);
			children.erase(it);

			for(auto& grandchild : ret->children)
				addChild(std::move(grandchild));

			return ret;
		}
	}
	assert(0);
}

std::vector<std::unique_ptr<Node>> const& Node::getChildren() const
{
	return children;
}

Scene* Node::getScene() const
{
	return scene;
}

bool Node::isEnabled() const
{
	return enabled;
}

bool Node::isHighlighted() const
{
	return highlighted;
}

void Node::setHighlighted(bool b)
{
	highlighted = b;
}
void Node::enable()
{
	enabled = true;
}

void Node::disable()
{
	enabled = false;
}

Node* Node::addChild(std::unique_ptr<Node>&& node, bool retainGlobalTransformation)
{
	auto desiredGlobalTransformation = node->getGlobalTransformation();
	node->parent = this;
	node->setScene(scene);
	auto ret = node.get();
	children.push_back(std::move(node));
	if(retainGlobalTransformation)
		ret->setGlobalTransformation(std::move(desiredGlobalTransformation));

	invalidateSceneCache();
	return ret;
}

void Node::addChildren(std::vector<std::unique_ptr<Node>>&& nodes, bool retainGlobalTransformation)
{
	children.reserve(children.size() + nodes.size());
	for(auto& node : nodes)
		addChild(std::move(node), retainGlobalTransformation);
}

std::unique_ptr<Node> Node::release()
{
	if(!parent)
		assert(0);
	return parent->releaseChild(this);
}

std::vector<std::unique_ptr<Node>> Node::releaseChildren()
{
	invalidateSceneCache();
	return std::move(children);
}

Bounds Node::getBounds() const
{
	Bounds bounds;
	for(auto const& child : children)
		bounds += child->getBounds();
	return bounds;
}

void Node::drawUI()
{
	IDGuard idGuard{this};
	if(!children.empty())
	{
		const float heightSubNodes = ImGui::GetTextLineHeightWithSpacing() * 5;
		const float heightSpacing = ImGui::GetTextLineHeightWithSpacing() * 1;
		ImGui::BeginChild("Node", {ImGui::GetTextLineHeightWithSpacing() * 22, heightSubNodes + heightSpacing});
		ImGui::Columns(2, nullptr, false);
		static int ctAllSubNodes = 0;
		ImGui::Text("All Sub Nodes (%i)", ctAllSubNodes);
		ImGui::BeginChild("###SubNodes", {-1, heightSubNodes}, true);
		ctAllSubNodes = 0;
		for(auto& node : children)
			node->recursive([&](Node* node){ ImGui::BulletText(node->name.get().data()); ctAllSubNodes++; });
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text("Direct Sub Nodes (%i)", children.size());
		ImGui::BeginChild("###DirectSubNodes", {-1, heightSubNodes}, true);
		for(auto& node : children)
			ImGui::BulletText(node->name.get().data());
		ImGui::EndChild();
		ImGui::Columns(1);
		ImGui::EndChild();
	}
}
