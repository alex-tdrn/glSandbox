#include "Scene.h"
#include "Prop.h"

#include <imgui.h>

Scene::Scene(std::vector<std::unique_ptr<Node>>&& nodes)
	:nodes(std::move(nodes))
{

}

Camera& Scene::getCamera()
{
	return camera;
}

std::forward_list<Prop const*> collectActiveProps(std::vector<std::unique_ptr<Node>> const& nodes)
{
	std::forward_list<Prop const*> activeProps;
	for(auto const& node : nodes)
	{
		Prop const* prop = dynamic_cast<Prop const*>(node.get());
		if(prop)
			activeProps.push_front(prop);
		activeProps.splice_after(activeProps.cend(), collectActiveProps(node->getChildren()));
	}
	return activeProps;
}
std::forward_list<Prop const*> Scene::getActiveProps() const
{
	return collectActiveProps(nodes);
}


//void Scene::drawUI(bool const showHeader)
//{
//	IDGuard idGuard{this};
//
//	if(!showHeader || ImGui::TreeNode(name.data()))
//	{
//		ImGui::Indent();
//		if(ImGui::ColorEdit3("Background", &backgroundColor.x, ImGuiColorEditFlags_NoInputs))
//			update();
//		std::string_view comboPreview = "None";
//		if(skybox)
//			comboPreview = skybox->getName();
//		if(ImGui::BeginCombo("Skybox", comboPreview.data()))
//		{
//			if(ImGui::Selectable("None", !skybox))
//			{
//				skybox = nullptr;
//				update();
//			}
//			if(ImGui::Selectable(resources::cubemaps::skybox.getName().data(), skybox == &resources::cubemaps::skybox))
//			{
//				skybox = &resources::cubemaps::skybox;
//				update();
//			}
//			if(ImGui::Selectable(resources::cubemaps::mp_blizzard.getName().data(), skybox == &resources::cubemaps::mp_blizzard))
//			{
//				skybox = &resources::cubemaps::mp_blizzard;
//				update();
//			}
//			ImGui::EndCombo();
//		}
//		if(camera.drawUI())
//			update();
//		auto generateListUI = [&](auto& data, std::optional<std::string_view const> const name = std::nullopt){
//		if(!name.has_value() || ImGui::TreeNode((*name).data()))
//		{
//			if(ImGui::Button("Disable All"))
//			{
//				update();
//				for(auto& entity : data)
//					entity.disable();
//			}
//			ImGui::SameLine();
//			if(ImGui::Button("Enable All"))
//			{
//				update();
//				for(auto& entity : data)
//					entity.enable();
//			}
//			if(ImGui::Button("Add New"))
//			{
//				update();
//				data.emplace_back();
//			}
//			int removeIdx = -1;
//			for(int i = 0; i < data.size(); i++)
//			{
//				if(ImGui::TreeNode(std::to_string(i).data()))
//				{
//					if(ImGui::Button("Remove"))
//						removeIdx = i;
//					if(data[i].drawUI())
//						update();
//					ImGui::TreePop();
//				}
//			}
//			if(removeIdx != -1)
//			{
//				data.erase(data.begin() + removeIdx);
//				update();
//			}
//			if(name.has_value())
//				ImGui::TreePop();
//			}
//		};
//		if(ImGui::CollapsingHeader("Actors"))
//		{
//			generateListUI(actors);
//		}
//		if(ImGui::CollapsingHeader("Lights"))
//		{
//			if(ImGui::Button("Disable All"))
//			{
//				update();
//				for(auto& light : directionalLights)
//					light.disable();
//				for(auto& light : pointLights)
//					light.disable();
//				for(auto& light : spotLights)
//					light.disable();
//			}
//			ImGui::SameLine();
//			if(ImGui::Button("Enable All"))
//			{
//				update();
//				for(auto& light : directionalLights)
//					light.enable();
//				for(auto& light : pointLights)
//					light.enable();
//				for(auto& light : spotLights)
//					light.enable();
//			}
//			generateListUI(directionalLights, "Directional Lights");
//			generateListUI(pointLights, "Point Lights");
//			generateListUI(spotLights, "Spot Lights");
//		}
//		ImGui::Unindent();
//		if(showHeader)
//			ImGui::TreePop();
//	}
//}