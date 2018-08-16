#include "Node.h"
#include "Util.h"
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

void Node::addChild(std::unique_ptr<Node>&& node)
{
	node->parent = this;
	node->setScene(scene);
	children.push_back(std::move(node));
	invalidateSceneCache();
}

void Node::addChildren(std::vector<std::unique_ptr<Node>>&& nodes)
{
	children.reserve(children.size() + nodes.size());
	for(auto& node : nodes)
		addChild(std::move(node));
}

std::unique_ptr<Node> Node::release()
{
	if(!parent)
		assert(0);
	return parent->releaseChild(this);
}

void Node::setTransformation(glm::mat4&& t)
{
	transformation = std::move(t);
}

glm::mat4 Node::getTransformation() const
{
	if(parent == nullptr)
		return transformation;
	else
		return parent->getTransformation() * transformation;
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
	ImGui::Text("Transformations");
	if(parent != nullptr)
	{
		ImGui::Text("Global");
		{
			drawMatrix(getTransformation());
			auto [t, r, s] = decomposeTransformation(getTransformation());
			auto e = glm::eulerAngles(r);
			ImGui::Text("%.2f %.2f %.2f T", t[0], t[1], t[2]);
			ImGui::Text("%.2f %.2f %.2f R", e[0], e[1], e[2]);
			ImGui::Text("%.2f %.2f %.2f S", s[0], s[1], s[2]);
		}
		ImGui::Text("Relative");
		{
			bool recomposeMatrix = false;
			bool uniformScale = false;
			drawEditableMatrix(transformation);
			auto[t, r, s] = decomposeTransformation(transformation);
			auto e = glm::eulerAngles(r);
			float scale = s[0];
			recomposeMatrix |= ImGui::DragFloat3("T", &t[0], 0.01f, 0, 0, "%.2f");
			recomposeMatrix |= ImGui::DragFloat3("R", &e[0], 0.01f, 0, 0, "%.2f");
			uniformScale |= ImGui::DragFloat("Scale uniformly", &scale, 0.01f, 0, 0, "%.2f");
			recomposeMatrix |= ImGui::DragFloat3("S", &s[0], 0.01f, 0, 0, "%.2f");
			if(recomposeMatrix | uniformScale)
			{
				if(uniformScale)
					s = glm::vec3(scale);
				transformation = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
			}
		}
	}
	else
	{
		ImGui::Text("Global");
		bool recomposeMatrix = false;
		bool uniformScale = false;
		drawEditableMatrix(transformation);
		auto[t, r, s] = decomposeTransformation(transformation);
		auto e = glm::eulerAngles(r);
		float scale = s[0];
		recomposeMatrix |= ImGui::DragFloat3("T", &t[0], 0.01f, 0, 0, "%.2f");
		recomposeMatrix |= ImGui::DragFloat3("R", &e[0], 0.01f, 0, 0, "%.2f");
		uniformScale |= ImGui::DragFloat("Scale uniformly", &scale, 0.01f, 0, 0, "%.2f");
		recomposeMatrix |= ImGui::DragFloat3("S", &s[0], 0.01f, 0, 0, "%.2f");
		if(recomposeMatrix | uniformScale)
		{
			if(uniformScale)
				s = glm::vec3(scale);
			transformation = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
		}
	}
	if(!children.empty())
	{
		ImGui::Text("Children");
	}
}
