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

std::string_view Node::getName() const
{
	return name.get();
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

std::vector<std::unique_ptr<Node>> Node::releaseChildren()
{
	invalidateSceneCache();
	return std::move(children);
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
	static bool showMatrix = true;
	static int relative = true;
	static bool uniformScale = true;
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Transformation");
	ImGui::SameLine();
	ImGui::RadioButton("Global", &relative, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Relative", &relative, 1);
	ImGui::SameLine();
	ImGui::Checkbox("Show Matrix", &showMatrix);
	ImGui::BeginChild("###Transformation", {ImGui::GetTextLineHeightWithSpacing() * 22, 0}, true);
	{
		glm::mat4 const& matrix = relative ? transformation : getTransformation();
		if(showMatrix)
		{
			ImGui::BeginChild("###Matrix", {0, ImGui::GetTextLineHeightWithSpacing() * 4}, false);
			ImGui::Columns(4, nullptr, false);
			for(int row = 0; row < 4; row++)
			{
				for(int col = 0; col < 4; col++)
				{
					ImGui::Text("%.2f", matrix[col][row]);
					ImGui::NextColumn();
				}
			}
			ImGui::Columns(1);
			ImGui::EndChild();
			ImGui::Separator();
		}
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matrix, scale, rotation, translation, skew, perspective);
		glm::vec4 angleAxis{glm::axis(rotation), glm::degrees(glm::angle(rotation))};
		if(relative)
		{
			float availWidth = ImGui::GetContentRegionAvailWidth() - ImGui::GetTextLineHeightWithSpacing() * 2;
			ImGui::PushItemWidth(-1);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Tr");
			ImGui::PushItemWidth(availWidth / 3.0);
			for(int i = 0 ; i < 3; i++)
			{
				ImGui::PushID(i);
				ImGui::SameLine();
				ImGui::DragFloat("###Tr", &translation[i], 0.01f);
				ImGui::PopID();
				if(i == 2)
					ImGui::PopItemWidth();
			}

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Ro");
			ImGui::SameLine();
			ImGui::PushItemWidth(availWidth / 4.0);
			ImGui::DragFloat("###RoAngle", &angleAxis[3], 1.00f);
			for(int i = 0; i < 3; i++)
			{
				ImGui::PushID(i);
				ImGui::SameLine();
				ImGui::DragFloat("###Ro", &angleAxis[i], 0.01f);
				ImGui::PopID();
				if(i == 2)
					ImGui::PopItemWidth();
			}

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Sc");
			ImGui::PushItemWidth(availWidth / 3.0);
			for(int i = 0; i < 3; i++)
			{
				ImGui::PushID(i);
				ImGui::SameLine();
				ImGui::DragFloat("###Sc", &scale[i], 0.01f);
				if(ImGui::IsItemActive() && ImGui::GetIO().KeyShift)
					scale = glm::vec3(scale[i]);
				ImGui::PopID();
				if(i == 2)
					ImGui::PopItemWidth();
			}
			ImGui::PopItemWidth();

			if(ImGui::IsAnyItemActive())
			{
				glm::mat4 Tr = glm::translate(glm::mat4(1.0f), translation);
				glm::mat4 Ro = glm::mat4_cast(glm::angleAxis(glm::radians(angleAxis[3]), glm::normalize(glm::vec3(angleAxis))));
				for(int i = 0; i < 3; i++)
					if(scale[i] == 0.0f)
						scale[i] = 0.01f;
				glm::mat4 Sc = glm::scale(glm::mat4(1.0f), scale);
				transformation = Tr * Ro * Sc;
			}
		}
	}
	ImGui::EndChild();
	
	if(!children.empty())
	{
		ImGui::Text("Children");
	}
}
