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

std::string const& Node::getName() const
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

void Node::setLocalTransformation(glm::mat4&& matrix)
{
	std::tie(localTranslation, localRotation, localScale) = matrixToComponents(matrix);
	auto l = componentsToMatrix(localTranslation, localRotation, localScale);
	//for(int i = 0; i < 4; i++)
		//for(int j = 0; j < 4; j++)
			//assert(std::abs(matrix[i][j] - l[i][j]) < 0.000'000'000'1f);
}

void Node::setLocalTransformation(glm::vec3 && t, glm::vec3 && r, glm::vec3 && s)
{
	localTranslation = std::move(t);
	localRotation = std::move(r);
	localScale = std::move(s);
}

glm::mat4 Node::getLocalTransformation() const
{
	return componentsToMatrix(localTranslation, localRotation, localScale);
}

glm::mat4 Node::getGlobalTransformation() const
{
	if(parent == nullptr)
		return getLocalTransformation();
	else
		return parent->getGlobalTransformation() * getLocalTransformation();
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
	static int local = true;
	static float heightTransformation = ImGui::GetTextLineHeightWithSpacing() * (showMatrix ? 10.5f : 6);
	const float heightSubNodes = ImGui::GetTextLineHeightWithSpacing() * 5;
	ImGui::BeginChild("Node", {ImGui::GetTextLineHeightWithSpacing() * 22, heightTransformation + heightSubNodes + ImGui::GetTextLineHeightWithSpacing() * 3});
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Transformation");
	ImGui::SameLine();
	ImGui::RadioButton("Global", &local, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Relative", &local, 1);
	ImGui::SameLine();
	ImGui::Checkbox("Show Matrix", &showMatrix);
	heightTransformation = ImGui::GetTextLineHeightWithSpacing() * (showMatrix ? 10.5f : 6);
	ImGui::BeginChild("###Transformation", {0, heightTransformation}, true);
	{
		glm::mat4 const& matrix = local ? getLocalTransformation() : getGlobalTransformation();
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

		auto[globalTranslation, globalRotation, globalScale] = matrixToComponents(getGlobalTransformation());
		float availWidth = ImGui::GetContentRegionAvailWidth() - ImGui::GetTextLineHeightWithSpacing() * 2;
		ImGui::PushItemWidth(-1);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Tr");
		ImGui::PushItemWidth(availWidth / 3.0);
		for(int i = 0 ; i < 3; i++)
		{
			ImGui::PushID(i);
			ImGui::SameLine();
			if(local)
				ImGui::DragFloat("###Tr", &localTranslation[i], 0.01f);
			else
				ImGui::Text("%.3f", globalTranslation[i]);
			ImGui::PopID();
			if(i == 2)
				ImGui::PopItemWidth();
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Ro");
		ImGui::SameLine();
		ImGui::PushItemWidth(availWidth / 3.0);
		for(int i = 0; i < 3; i++)
		{
			ImGui::PushID(i);
			ImGui::SameLine();
			if(local)
				ImGui::DragFloat("###Ro", &localRotation[i], 1.0f);
			else
				ImGui::Text("%.3f", globalRotation[i]);

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
			if(local)
				ImGui::DragFloat("###Sc", &localScale[i], 0.01f);
			else
				ImGui::Text("%.3f", globalScale[i]);

			if(local && ImGui::IsItemActive() && ImGui::GetIO().KeyShift)
				localScale = glm::vec3(localScale[i]);
			ImGui::PopID();
			if(i == 2)
				ImGui::PopItemWidth();
		}
		ImGui::PopItemWidth();

		auto[bMin, bMax] = getBounds().getValues();
		ImGui::Text("Bounds (%.1f, %.1f, %.1f) - (%.1f, %.1f, %.1f)", bMin.x, bMin.y, bMin.z, bMax.x, bMax.y, bMax.z);
	}
	ImGui::EndChild();
	
	if(!children.empty())
	{
		ImGui::Columns(2, nullptr, false);
		static int ctAllSubNodes = 0;
		ImGui::Text("All Sub Nodes (%i)", ctAllSubNodes);
		ImGui::BeginChild("###SubNodes", {-1, heightSubNodes}, true);
		ctAllSubNodes = 0;
		for(auto& node : children)
			node->recursive([&](Node* node){ ImGui::BulletText(node->getName().data()); ctAllSubNodes++; });
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text("Direct Sub Nodes (%i)", children.size());
		ImGui::BeginChild("###DirectSubNodes", {-1, heightSubNodes}, true);
		for(auto& node : children)
			ImGui::BulletText(node->getName().data());
		ImGui::EndChild();
		ImGui::Columns(1);
	}
	ImGui::EndChild();
}
