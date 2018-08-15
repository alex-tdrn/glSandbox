#include "Scene.h"
#include "Prop.h"
#include "Util.h"

#include <imgui.h>
#include <variant>

Scene::Scene(std::vector<std::unique_ptr<Node>>&& rootNodes)
	:rootNodes(std::move(rootNodes))
{
	for(auto& node : rootNodes)
		node->setScene(this);

	fitToIdealSize();
}

void Scene::updatePrimaryCache() const
{
	auto fillCache = [&](std::vector<std::unique_ptr<Node>> const& nodes, auto& fillCache) -> void{
		for(auto& node : nodes)
		{
			if(auto prop = dynamic_cast<Prop*>(node.get()); prop)
				primaryCache.props.push_back(prop);
			else
				primaryCache.abstractNodes.push_back(node.get());
			fillCache(node->getChildren(), fillCache);
		}
	};
	fillCache(rootNodes, fillCache);
	primaryCache.dirty = false;
}

void Scene::updateSecondaryCache() const
{
	if(primaryCache.dirty)
		updatePrimaryCache();

	auto copyActiveNodes = [](auto& primaryVec, auto& secondaryVec){
		secondaryVec.clear();
		for(auto elem : primaryVec)
			if(elem->enabled)
				secondaryVec.push_back(elem);
	};
	copyActiveNodes(primaryCache.abstractNodes, secondaryCache.abstractNodes);
	copyActiveNodes(primaryCache.props, secondaryCache.props);

}

void Scene::add(std::unique_ptr<Node>&& node)
{
	node->setScene(this);
	rootNodes.push_back(std::move(node));
	primaryCache.dirty = true;
}

void Scene::remove(Node* node)
{
	rootNodes.erase(std::remove_if(rootNodes.begin(), rootNodes.end(), [&](std::unique_ptr<Node> const& val){ return val.get() == node; }), rootNodes.end());
	primaryCache.dirty = true;
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

void Scene::fitToIdealSize()
{
	Bounds bounds;
	for(auto const& node : rootNodes)
		bounds += node->getBounds();
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
	for(auto& node : rootNodes)
		node->setTransformation(s * t * node->getTransformation());
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
		auto areYouSureModal = [](bool justActivated, auto yes) -> bool{
			bool choice = false;
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
					choice = true;
				}
				ImGui::SameLine();
				if(ImGui::Button("No", buttonSize))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			return choice;
		};
		int id = 0;
		ImGui::Text("Abstract Nodes");
		ImGui::BeginChild("###Abstract Nodes", {0, scrollAreaHeight}, true);
		for(auto node : getAll<Node>())
		{
			ImGui::PushID(id++);
			bool active = false;
			if(std::holds_alternative<Node*>(selected))
				active = std::get<Node*>(selected) == node;
			if(ImGui::Selectable(node->name.get().data(), active))
				selected = node;

			if(ImGui::BeginPopupContextItem())
			{
				if(ImGui::Selectable("Add"))
					node->add(std::make_unique<Node>());
				ImGui::EndPopup();
			}

			bool deleteAndTransferChildNodes = false;
			if(ImGui::BeginPopupContextItem())
			{
				deleteAndTransferChildNodes = ImGui::Selectable("Delete and transfer child nodes");
				ImGui::EndPopup();
			}
			if(areYouSureModal(deleteAndTransferChildNodes, [&](){
				if(active)
					selected = static_cast<Node*>(nullptr);
				node->deleteAndTransferChildNodes();
			}))
				break;

			bool deleteRecursively = false;
			if(ImGui::BeginPopupContextItem())
			{
				deleteRecursively = ImGui::Selectable("Delete recursively");
				ImGui::EndPopup();
			}
			if(areYouSureModal(deleteRecursively, [&](){
				if(active)
					selected = static_cast<Node*>(nullptr);
				node->deleteRecursively();
			}))
				break;

			ImGui::PopID();
		}
		ImGui::EndChild();

		ImGui::Text("Props");
		ImGui::BeginChild("###Props", {0, scrollAreaHeight}, true);
		for(auto prop : getAll<Prop>())
		{
			ImGui::PushID(id++);
			bool active = false;
			if(std::holds_alternative<Prop*>(selected))
				active = std::get<Prop*>(selected) == prop;
			if(ImGui::Selectable(prop->name.get().data(), active))
				selected = prop;
			ImGui::PopID();
		}
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
