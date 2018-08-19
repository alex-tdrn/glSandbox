#include "Prop.h"
#include "Resources.h"

Prop::Prop(std::shared_ptr<Mesh> const& mesh)
	:mesh(mesh)
{

}

Prop::Prop(std::shared_ptr<Mesh>&& mesh)
	:mesh(std::move(mesh))
{

}

Mesh& Prop::getMesh() const
{
	return *mesh;
}

Bounds Prop::getBounds() const
{
	return mesh->getBounds() * getGlobalTransformation();
}

std::string const& Prop::getName() const
{
	return name.get();
}

void Prop::drawUI()
{
	Node::drawUI();
	ImGui::BeginChild("###Prop", {ImGui::GetTextLineHeightWithSpacing() * 22, 0});
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Mesh");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if(ImGui::BeginCombo("###Mesh", mesh->name.get().data()))
	{
		int id = 0;
		for(auto& m : res::meshes::getAll())
		{
			ImGui::PushID(id++);
			bool isSelected = mesh.get() == m.get();
			if(ImGui::Selectable(m->name.get().data(), &isSelected))
				mesh = m;
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	ImGui::EndChild();
}

