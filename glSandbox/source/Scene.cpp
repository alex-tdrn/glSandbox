#include "Scene.h"
#include "Prop.h"
#include "Util.h"
#include "Camera.h"

#include <imgui.h>
#include <variant>
#include <set>

Scene::Scene()
{
	root->setScene(this);
	root->addChild(std::make_unique<Camera>(), true);
}

Scene::Scene(Scene &&other)
	:root(std::move(other.root))
{
	root->setScene(this);
	if(getAll<Camera>().empty())
		root->addChild(std::make_unique<Camera>(), true);
}
Scene::Scene(std::unique_ptr<Node>&& root)
	:root(std::move(root))
{
	this->root->setScene(this);
	fitToIdealSize();
	if(getAll<Camera>().empty())
		this->root->addChild(std::make_unique<Camera>(), true);
}

void Scene::updateCache() const
{
	cache.transformedNodes.clear();
	cache.cameras.clear();
	cache.props.clear();
	for(auto& node : root->getChildren())
	{
		node.get()->recursive([&](Node* node){
			if(auto prop = dynamic_cast<Prop*>(node); prop)
				cache.props.push_back(prop);
			else if(auto camera = dynamic_cast<Camera*>(node); camera)
				cache.cameras.push_back(camera);
			else if(auto transformedNode = dynamic_cast<TransformedNode*>(node); transformedNode)
				cache.transformedNodes.push_back(transformedNode);
		});
	}
	cache.dirty = false;
}

void Scene::cacheOutdated() const
{
	cache.dirty = true;
}

Node* Scene::getRoot() const
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
	root->setLocalTransformation(s * t * glm::mat4(1.0f));// root->getGlobalTransformation());
}

void Scene::drawUI()
{
	IDGuard idGuard{this};
	
	ImGui::ColorEdit3("Background", &backgroundColor.x, ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	if(ImGui::Button("Fit To Ideal Size"))
		fitToIdealSize();
	ImGui::NewLine();

	float const scrollAreaWidth = ImGui::GetTextLineHeightWithSpacing() * 15;
	static int hierarchyView = 0;

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(-1, scrollAreaWidth);

	static Node* selected;
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Nodes");
	ImGui::SameLine();
	if(ImGui::RadioButton("Category", &hierarchyView, 0))
		selected = nullptr;
	ImGui::SameLine();
	if(ImGui::RadioButton("Hierarchy", &hierarchyView, 1))
		selected = nullptr;
	ImGui::BeginChild("###Nodes");

	std::set<Node*> nodesMarkedForHighlighting;
	std::set<Node*> nodesMarkedForShallowRemove;
	auto modal = [](bool justActivated, std::string_view title){
		bool answer = false;
		if(justActivated)
			ImGui::OpenPopup(title.data());
		if(ImGui::BeginPopupModal(title.data(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize({100, ImGui::GetTextLineHeight() * 5});
			ImGui::Text("Are you sure?");
			ImVec2 buttonSize{ImGui::GetContentRegionAvailWidth() * 0.5f, ImGui::GetContentRegionAvail().y};
			if(ImGui::Button("Yes", buttonSize))
			{
				answer = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if(ImGui::Button("No", buttonSize))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		return answer;
	};
	auto actionsAdd = [](Node* node){
		if(ImGui::BeginMenu("Add..."))
		{
			if(ImGui::MenuItem("Transformed Node"))
				node->addChild(std::make_unique<TransformedNode>());
			if(ImGui::MenuItem("Camera"))
				node->addChild(std::make_unique<Camera>());
			if(ImGui::MenuItem("Prop"))
				node->addChild(std::make_unique<Prop>());
			ImGui::EndMenu();
		}
	};
	auto actionsEnableDisable = [](Node* node){
		if(ImGui::Selectable("Enable"))
			node->enable();
		if(ImGui::Selectable("Disable"))
			node->disable();
		if(ImGui::Selectable("Recursive Enable"))
			node->recursive(&Node::enable);
		if(ImGui::Selectable("Recursive Disable"))
			node->recursive(&Node::disable);
	};
	auto drawRootContextMenu = [&](){
		bool removeAllChildren = false;
		if(ImGui::BeginPopupContextItem())
		{
			actionsAdd(root.get());
			actionsEnableDisable(root.get());
			ImGui::Separator();
			removeAllChildren = ImGui::Selectable("Remove All Children");
			ImGui::EndPopup();
		}
		if(modal(removeAllChildren, "Remove All Children"))
		{
			for(auto& child : root->getChildren())
			{
				child->recursive([&](Node* node){
					nodesMarkedForShallowRemove.insert(node);
				});
			}
		}
	};
	auto drawNodeContextMenu = [&](Node* node){		
		bool remove = false;
		bool removeRecursive = false;
		bool removeChildren = false;
		if(ImGui::BeginPopupContextItem())
		{
			actionsAdd(node);
			actionsEnableDisable(node);
			ImGui::Separator();
			remove = ImGui::Selectable("Remove");
			removeRecursive = ImGui::Selectable("Recursive Remove");
			removeChildren = ImGui::Selectable("Remove All Children");
			ImGui::EndPopup();
		}
		if(modal(remove, "Remove"))
		{
			nodesMarkedForShallowRemove.insert(node);
		}
		if(modal(removeRecursive, "Recursive Remove"))
		{
			node->recursive([&](Node* node){
				nodesMarkedForShallowRemove.insert(node); 
			});
		}
		if(modal(removeChildren, "Remove All Children"))
		{
			for(auto& child : node->getChildren())
			{
				child->recursive([&](Node* node){ 
					nodesMarkedForShallowRemove.insert(node); 
				});
			}
		}
	};
	root->recursive([](Node* node){ node->setHighlighted(false); });
	if(hierarchyView)
	{
		auto drawNode = [&](Node* node, bool root = false){
			int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if(root)
				flags = flags | ImGuiTreeNodeFlags_DefaultOpen;
			if(node->getChildren().empty())
				flags = flags | ImGuiTreeNodeFlags_Leaf;
			if(selected == node)
				flags = flags | ImGuiTreeNodeFlags_Selected;
			bool expandNode = ImGui::TreeNodeEx(((root ? "Root Node" : node->getName().data()) + std::string(node->enabled ? "" : " *")).data(), flags);
			if(ImGui::IsItemClicked())
				selected = node;
			if(ImGui::IsItemHovered() || selected == node)
				node->recursive([&](Node* node){ nodesMarkedForHighlighting.insert(node); });
			else
				node->setHighlighted(false);
			return expandNode;
		};
		auto drawSubtree = [&, id = 0](Node* node, auto& drawSubtree)mutable -> void{
			ImGui::PushID(id++);
			bool expandNode = drawNode(node);
			drawNodeContextMenu(node);
			ImGui::PopID();
			if(expandNode)
			{
				for(auto& child : node->getChildren())
					drawSubtree(child.get(), drawSubtree);
				ImGui::TreePop();
			}
		};

		bool expandNode = drawNode(root.get(), true);
		drawRootContextMenu();
		if(expandNode)
		{
			for(auto& node : root->getChildren())
				drawSubtree(node.get(), drawSubtree);
			ImGui::TreePop();
		}
	}
	else
	{
		auto drawNode = [&](Node* node, bool root = false){
			if(ImGui::Selectable(((root ? "Root Node" : node->getName().data()) + std::string(node->enabled ? "" : " *")).data(), selected == node))
				selected = node;
			if(ImGui::IsItemHovered() || selected == node)
				node->recursive([&](Node* node){ nodesMarkedForHighlighting.insert(node); });
			else
				node->setHighlighted(false);
		};
		auto drawList = [&](auto const& nodes){

			if(ImGui::IsAnyItemActive() && ImGui::IsMouseHoveringWindow() && !ImGui::IsMouseDragging())//check for mouse click inside window
				selected = nullptr;

			int id = 0;
			for(auto node : nodes)
			{
				ImGui::PushID(id++);
				drawNode(node);
				drawNodeContextMenu(node);
				ImGui::PopID();
			}
			
		};
		float const scrollAreaHeight = ImGui::GetTextLineHeightWithSpacing() * 8;

		ImGui::Text("Abstract Nodes");
		ImGui::BeginChild("###Abstract Nodes", {0, scrollAreaHeight}, true);
		drawNode(root.get(), true);
		drawRootContextMenu();
		drawList(getAll<TransformedNode>());
		ImGui::EndChild();

		ImGui::Text("Props");
		ImGui::BeginChild("###Props", {0, scrollAreaHeight}, true);
		drawList(getAll<Prop>());
		ImGui::EndChild();

		ImGui::Text("Cameras");
		ImGui::BeginChild("###Cameras", {0, scrollAreaHeight}, true);
		drawList(getAll<Camera>());
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
	}
	for(auto node : nodesMarkedForHighlighting)
		node->setHighlighted(true);
	for(auto node : nodesMarkedForShallowRemove)
	{
		if(selected == node)
			selected = nullptr;
		node->release();
	}

	if(ImGui::IsAnyItemActive() && ImGui::IsMouseHoveringWindow() && !ImGui::IsMouseDragging())//check for mouse click inside window
		selected = nullptr;
	ImGui::EndChild();

	ImGui::NextColumn();
	ImGui::Text(selected ? selected->getName().data() : "No selection...");
	ImGui::BeginChild("###Edit Node", {0, 0}, true);
	if(selected) selected->drawUI();
	ImGui::EndChild();

	ImGui::Columns(1);
}
