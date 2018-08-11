#include "Node.h"
#include "Util.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
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

glm::mat4 Node::getTransformation() const
{
	if(parent == nullptr)
		return transformation;
	else
		return transformation * parent->getTransformation();
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
				ImGui::DragFloat3("Translation", &t[0], 0.01f, 0, 0, "%.2f");
				ImGui::DragFloat3("Scale", &s[0], 0.01f, 0, 0, "%.2f");
			}
			ImGui::Text("Relative");
			{
				bool valueChanged = !drawEditableMatrix(transformation);
				auto[t, r, s] = decomposeTransformation(transformation);
				valueChanged |= ImGui::DragFloat3("Translation", &t[0], 0.01f, 0, 0, "%.2f");
				valueChanged |= ImGui::DragFloat3("Scale", &s[0], 0.01f, 0, 0, "%.2f");
				if(valueChanged)
				{
					transformation = glm::mat4(1.0f);
					transformation = glm::translate(transformation, t);
					transformation = glm::scale(transformation, s);
				}
			}
		}
		else
		{
			ImGui::Text("Global");
			bool valueChanged = !drawEditableMatrix(transformation);
			auto[t, r, s] = decomposeTransformation(transformation);
			valueChanged |= ImGui::DragFloat3("Translation", &t[0], 0.01f, 0, 0, "%.2f");
			valueChanged |= ImGui::DragFloat3("Scale", &s[0], 0.01f, 0, 0, "%.2f");
			if(valueChanged)
			{
				transformation = glm::mat4(1.0f);
				transformation = glm::translate(transformation, t);
				transformation = glm::scale(transformation, s);
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
