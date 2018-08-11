#include "Node.h"
#include "Util.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <algorithm>

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

void Node::addChild(std::unique_ptr<Node>&& node)
{
	node->parent = this;
	children.push_back(std::move(node));
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

void Node::drawUI()
{
	IDGuard idGuard{this};
	if(ImGui::TreeNode(name.get().data()))
	{
		ImGui::Text("Transformations");
		ImGui::Indent();
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
				drawEditableMatrix(transformation);
				auto[t, r, s] = decomposeTransformation(transformation);
				auto e = glm::eulerAngles(r);
				recomposeMatrix |= ImGui::DragFloat3("T", &t[0], 0.01f, 0, 0, "%.2f");
				recomposeMatrix |= ImGui::DragFloat3("R", &e[0], 0.01f, 0, 0, "%.2f");
				recomposeMatrix |= ImGui::DragFloat3("S", &s[0], 0.01f, 0, 0, "%.2f");
				if(recomposeMatrix)
				{
					transformation = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
				}
			}
		}
		else
		{
			ImGui::Text("Global");
			bool recomposeMatrix = false;
			drawEditableMatrix(transformation);
			auto[t, r, s] = decomposeTransformation(transformation);
			auto e = glm::eulerAngles(r);
			recomposeMatrix |= ImGui::DragFloat3("T", &t[0], 0.01f, 0, 0, "%.2f");
			recomposeMatrix |= ImGui::DragFloat3("R", &e[0], 0.01f, 0, 0, "%.2f");
			recomposeMatrix |= ImGui::DragFloat3("S", &s[0], 0.01f, 0, 0, "%.2f");
			if(recomposeMatrix)
			{
				transformation = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
			}
		}
		ImGui::Unindent();
		if(!children.empty())
		{
			ImGui::Text("Children");
			for(auto& child : children)
				child->drawUI();
		}
		ImGui::TreePop();
	}
}
