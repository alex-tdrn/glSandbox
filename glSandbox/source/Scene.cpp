#include "Scene.h"
#include "Prop.h"
#include "Util.h"

#include <imgui.h>
#include <variant>
#include <set>

Scene::Scene(std::unique_ptr<Node>&& root)
	:root(std::move(root))
{
	this->root->setScene(this);
	fitToIdealSize();
}

void Scene::updateCache() const
{
	cache.abstractNodes.clear();
	cache.props.clear();
	for(auto& node : root->getChildren())
	{
		node.get()->recursive([&](Node* node){
			if(auto prop = dynamic_cast<Prop*>(node); prop)
				cache.props.push_back(prop);
			else
				cache.abstractNodes.push_back(node);
		});
	}
	cache.dirty = false;
}

void Scene::cacheOutdated() const
{
	cache.dirty = true;
}

Node * Scene::getRoot() const
{
	return root.get();
}

std::vector<DirectionalLight> const& Scene::getDirectionalLights() const
{
	return directionalLights;
}

std::vector<PointLight> const& Scene::getPointLights() const
{
	return pointLights;
}

std::vector<SpotLight> const& Scene::getSpotLights() const
{
	return spotLights;
}

glm::vec3 const& Scene::getBackground() const
{
	return backgroundColor;
}

Camera& Scene::getCamera()
{
	return camera;
}

Camera const& Scene::getCamera() const
{
	return camera;
}

void Scene::fitToIdealSize() const
{
	Bounds bounds = root->getBounds();
	if(bounds.empty())
		return;

	auto[min, max] = bounds.getValues();

	glm::vec3 centroid = (min + max) * 0.5f;
	glm::vec3 translation = -centroid;
	min += translation;
	max += translation;

	glm::vec3 absMax;
	for(int i = 0; i < 3; i++)
		absMax[i] = std::max(std::abs(min[i]), std::abs(max[i]));
	float const currentSize = std::max({absMax[0], absMax[1], absMax[2]});
	float const scale = idealSize / currentSize;
	glm::mat4 t = glm::translate(glm::mat4{1.0f}, translation);
	glm::mat4 s = glm::scale(glm::mat4{1.0f}, glm::vec3{scale});

	root->setTransformation(s * t * root->getTransformation());
}

template <typename T>
void drawAll(Scene* scene, std::variant<Node*, Prop*>& selected, std::vector<Node*>& nodesMarkedForHighlighting)
{
	auto areYouSureModal = [](bool justActivated, auto yes){
		if(justActivated)
			ImGui::OpenPopup("Are you sure?");
		if(ImGui::BeginPopupModal("Are you sure?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize({100, ImGui::GetTextLineHeight() * 5});
			ImVec2 buttonSize{ImGui::GetContentRegionAvailWidth() * 0.5f, ImGui::GetContentRegionAvail().y};
			if(ImGui::Button("Yes", buttonSize))
			{
				yes();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if(ImGui::Button("No", buttonSize))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	};
	std::vector<Node*> nodesMarkedForRecursiveRemove;
	if(ImGui::IsAnyItemActive() && ImGui::IsMouseHoveringWindow())//check for mouse click inside window
		selected = static_cast<Node*>(nullptr);
	int id = 0;
	if(std::is_same_v<T, Node>)//draw root node first
	{
		Node* root = scene->getRoot();
		bool current = false;
		if(std::holds_alternative<Node*>(selected))
			current = std::get<Node*>(selected) == root;
		if(ImGui::Selectable("Root Node", current))
			selected = root;
		if(ImGui::IsItemHovered() || current)
			nodesMarkedForHighlighting.push_back(root);
		else
			root->setHighlighted(false);

		bool removeAllChildren = false;
		if(ImGui::BeginPopupContextItem())
		{
			if(ImGui::BeginMenu("Add..."))
			{
				if(ImGui::MenuItem("Abstract Node"))
					root->addChild(std::make_unique<Node>());
				if(ImGui::MenuItem("Prop"))
					root->addChild(std::make_unique<Prop>());
				ImGui::EndMenu();
			}
			if(ImGui::Selectable("Enable"))
				root->enable();
			if(ImGui::Selectable("Disable"))
				root->disable();
			if(ImGui::Selectable("Recursive Enable"))
				root->recursive(&Node::enable);
			if(ImGui::Selectable("Recursive Disable"))
				root->recursive(&Node::disable);
			ImGui::Separator();
			removeAllChildren = ImGui::Selectable("Remove All Children");
			ImGui::EndPopup();
		}
		ImGui::PushID("Remove All Children");
		areYouSureModal(removeAllChildren, [&, root]()mutable{
			root->releaseChildren();
		});
		ImGui::PopID();
	}
	for(auto node : scene->getAll<T>())
	{
		ImGui::PushID(id++);
		bool current = false;
		if(std::holds_alternative<T*>(selected))
			current = std::get<T*>(selected) == node;
		if(ImGui::Selectable(node->name.get().data(), current))
			selected = node;
		if(ImGui::IsItemHovered() || current)
			nodesMarkedForHighlighting.push_back(node);
		else
			node->setHighlighted(false);

		bool remove = false;
		bool recursiveRemove = false;
		bool removeAllChildren = false;
		if(ImGui::BeginPopupContextItem())
		{
			if(ImGui::BeginMenu("Add..."))
			{
				if(ImGui::MenuItem("Abstract Node"))
					node->addChild(std::make_unique<Node>());
				if(ImGui::MenuItem("Prop"))
					node->addChild(std::make_unique<Prop>());
				ImGui::EndMenu();
			}
			if(ImGui::Selectable("Enable"))
				node->enable();
			if(ImGui::Selectable("Disable"))
				node->disable();
			if(ImGui::Selectable("Recursive Enable"))
				node->recursive(&Node::enable);
			if(ImGui::Selectable("Recursive Disable"))
				node->recursive(&Node::disable);
			ImGui::Separator();
			remove = ImGui::Selectable("Remove");
			recursiveRemove = ImGui::Selectable("Recursive Remove");
			removeAllChildren = ImGui::Selectable("Remove All Children");
			ImGui::EndPopup();
		}
		ImGui::PushID("Remove");
		areYouSureModal(remove, [&, node]()mutable{
			if(current)
				selected = static_cast<Node*>(nullptr);
			node->release();
		});
		ImGui::PopID();
		ImGui::PushID("Recursive Remove");
		areYouSureModal(recursiveRemove, [&, node]()mutable{
			if(current)
				selected = static_cast<Node*>(nullptr);
			nodesMarkedForRecursiveRemove.push_back(node);
		});
		ImGui::PopID();
		ImGui::PushID("Remove All Children");
		areYouSureModal(removeAllChildren, [&, node]()mutable{
			for(auto& childNode : node->getChildren())
				nodesMarkedForRecursiveRemove.push_back(childNode.get());
		});
		ImGui::PopID();

		ImGui::PopID();
	}

	if(!nodesMarkedForRecursiveRemove.empty())
	{
		std::set<Node*> nodesMarkedForShallowRemove;
		for(auto node : nodesMarkedForRecursiveRemove)
			node->recursive([&](Node* node){ nodesMarkedForShallowRemove.insert(node); });

		for(auto node : nodesMarkedForShallowRemove)
			node->release();
	}
	

}

void Scene::drawUI()
{
	static std::variant<Node*, Prop*> selected;
	
	IDGuard idGuard{this};
	ImGui::ColorEdit3("Background", &backgroundColor.x, ImGuiColorEditFlags_NoInputs);
	ImGui::NewLine();

	root->recursive([](Node* node){ node->setHighlighted(false); });

	float const scrollAreaHeight = ImGui::GetTextLineHeight() * 10;
	float const scrollAreaWidth = ImGui::GetTextLineHeight() * 20;
	static bool hierarchyView = false;

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(-1, scrollAreaWidth);

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Nodes");
	ImGui::SameLine();
	if(ImGui::Checkbox("Hierarchy View", &hierarchyView))
		selected = static_cast<Node*>(nullptr);
	ImGui::BeginChild("###Nodes");
	if(ImGui::IsItemActive())//check for mouse click inside window
		selected = static_cast<Node*>(nullptr);
	if(hierarchyView)
	{

	}
	else
	{
		std::vector<Node*> nodesMarkedForHighlighting;

		ImGui::Text("Abstract Nodes");
		ImGui::BeginChild("###Abstract Nodes", {0, scrollAreaHeight}, true);
		drawAll<Node>(this, selected, nodesMarkedForHighlighting);
		ImGui::EndChild();

		ImGui::Text("Props");
		ImGui::BeginChild("###Props", {0, scrollAreaHeight}, true);
		drawAll<Prop>(this, selected, nodesMarkedForHighlighting);
		ImGui::EndChild();

		ImGui::Text("Cameras");
		ImGui::BeginChild("###Cameras", {0, scrollAreaHeight}, true);
		ImGui::EndChild();

		ImGui::Text("Directional Lights");
		ImGui::BeginChild("###Directional Lights", {0, scrollAreaHeight}, true);
		ImGui::EndChild();

		ImGui::Text("Spot Lights");
		ImGui::BeginChild("###Spot Lights", {0, scrollAreaHeight}, true);
		ImGui::EndChild();

		ImGui::Text("Point Lights");
		ImGui::BeginChild("###Point Lights", {0, scrollAreaHeight}, true);
		ImGui::EndChild();
		for(auto node : nodesMarkedForHighlighting)
			node->recursive([](Node* node){ node->setHighlighted(true); });
	}

	if(ImGui::IsAnyItemActive() && ImGui::IsMouseHoveringWindow())//check for mouse click inside window
		selected = static_cast<Node*>(nullptr);
	ImGui::EndChild();
	ImGui::NextColumn();
	std::visit([&](auto selected){
		ImGui::Text(selected ? selected->name.get().data() : "No selection...");
		ImGui::BeginChild("###Edit Node", {0, 0}, true);
		if(selected) selected->drawUI();
		ImGui::EndChild();
	}, selected);

	ImGui::Columns(1);
}
