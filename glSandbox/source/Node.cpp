#include "Node.h"
#include "Util.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <algorithm>

Node::Node(Node&& other)
	:children(std::move(other.children))
{
	for(auto& child : children)
		child->parent = this;
}

void Node::invalidateSceneCache(int id)
{
	if(scene)
	{
		if(id == 1)
			scene->primaryCache.dirty = true;
		else if(id == 2)
			scene->secondaryCache.dirty = true;
	}
}

void Node::setScene(Scene * scene)
{
	this->scene = scene;
}

void Node::enable()
{
	enabled = true;
	for(auto& child : children)
		child->enable();
	invalidateSceneCache(2);
}

void Node::disable()
{
	enabled = false;
	for(auto& child : children)
		child->disable();
	invalidateSceneCache(2);
}

void Node::removeFromParent()
{
	if(parent)
		parent->remove(this);
	else if(scene)
		scene->remove(this);
}

void Node::remove(Node* node)
{
	children.erase(std::remove_if(children.begin(), children.end(), [&](std::unique_ptr<Node> const& val){ return val.get() == node; }), children.end());
	invalidateSceneCache(1);
}

void Node::deleteAndTransferChildNodes()
{
	if(parent)
		for(auto& childNode : children)
			parent->add(std::move(childNode));
	else if(scene)
		for(auto& childNode : children)
			scene->add(std::move(childNode));
	removeFromParent();
}

void Node::deleteRecursively()
{
	removeFromParent();
}

std::vector<std::unique_ptr<Node>> const& Node::getChildren() const
{
	return children;
}

void Node::add(std::unique_ptr<Node>&& node)
{
	node->parent = this;
	node->setScene(scene);
	children.push_back(std::move(node));
	invalidateSceneCache(1);
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
