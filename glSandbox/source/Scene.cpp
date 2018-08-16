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
	auto fillCache = [&](std::unique_ptr<Node> const& node, auto& fillCache) -> void{
		if(auto prop = dynamic_cast<Prop*>(node.get()); prop)
			cache.props.push_back(prop);
		else
			cache.abstractNodes.push_back(node.get());

		for(auto const& child : node->children)
			fillCache(child, fillCache);
	};
	fillCache(root, fillCache);
	cache.dirty = false;
}

void Scene::cacheOutdated() const
{
	cache.dirty = true;
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
void drawAll(Scene* scene, std::variant<Node*, Prop*>& selected)
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

	int id = 0;
	for(auto node : scene->getAll<T>())
	{
		ImGui::PushID(id++);
		bool active = false;
		if(std::holds_alternative<T*>(selected))
			active = std::get<T*>(selected) == node;
		if(ImGui::Selectable(node->name.get().data(), active))
			selected = node;

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
			ImGui::EndPopup();
		}

		bool remove = false;
		if(ImGui::BeginPopupContextItem())
		{
			remove = ImGui::Selectable("Remove");
			ImGui::EndPopup();
		}
		ImGui::PushID("Remove");
		areYouSureModal(remove, [&, node]()mutable{
			if(active)
				selected = static_cast<Node*>(nullptr);
			node->release();
		});
		ImGui::PopID();

		bool recursiveRemove = false;
		if(ImGui::BeginPopupContextItem())
		{
			recursiveRemove = ImGui::Selectable("Recursive Remove");
			ImGui::EndPopup();
		}
		ImGui::PushID("Recursive Remove");
		areYouSureModal(recursiveRemove, [&, node]()mutable{
			if(active)
				selected = static_cast<Node*>(nullptr);
			nodesMarkedForRecursiveRemove.push_back(node);
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
	IDGuard idGuard{this};
	ImGui::ColorEdit3("Background", &backgroundColor.x, ImGuiColorEditFlags_NoInputs);
	ImGui::NewLine();

	float const scrollAreaHeight = ImGui::GetTextLineHeight() * 10;
	float const scrollAreaWidth = ImGui::GetTextLineHeight() * 20;
	static std::variant<Node*, Prop*> selected;
	static bool hierarchyView = false;

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(-1, scrollAreaWidth);

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Nodes");
	ImGui::SameLine();
	if(ImGui::Checkbox("Hierarchy View", &hierarchyView))
		selected = static_cast<Node*>(nullptr);
	ImGui::BeginChild("###Nodes");
	if(hierarchyView)
	{

	}
	else
	{

		
		ImGui::Text("Abstract Nodes");
		ImGui::BeginChild("###Abstract Nodes", {0, scrollAreaHeight}, true);
		drawAll<Node>(this, selected);
		ImGui::EndChild();

		ImGui::Text("Props");
		ImGui::BeginChild("###Props", {0, scrollAreaHeight}, true);
		drawAll<Prop>(this, selected);
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
	}

	ImGui::EndChild();

	ImGui::NextColumn();
	std::visit([](auto selected){
		ImGui::Text(selected ? selected->name.get().data() : "No selection...");
		ImGui::BeginChild("###Edit Node", {0, 0}, true);
		if(selected) selected->drawUI();
		ImGui::EndChild();
	}, selected);

	ImGui::Columns(1);
}
