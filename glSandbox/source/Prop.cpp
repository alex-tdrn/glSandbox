#include "Prop.h"
#include "Resources.h"
#include "Mesh.h"
#include "Grid.h"
#include "SierpinskiTriangle.h"
#include "SierpinskiTetrahedon.h"
#include "SierpinskiCarpet.h"

Prop::Prop(Mesh* mesh)
	:staticMesh(mesh)
{

}

Prop::Prop(std::unique_ptr<ProceduralMesh>&& mesh)
	: proceduralMesh(std::move(mesh))
{

}

Mesh& Prop::getMesh() const
{
	if(staticMesh)
		return *staticMesh;
	else if(proceduralMesh)
		return *proceduralMesh->get();
	else return *res::meshes::boxWireframe();
}

Bounds Prop::getBounds() const
{
	Bounds childBounds = Node::getBounds();
	return getMesh().getBounds() * getGlobalTransformation() + childBounds;
}

void Prop::setName(std::string const& name)
{
	this->name.set(name);
}

std::string const& Prop::getName() const
{
	return name.get();
}
template <typename PM>
void addProceduralMeshItem(std::unique_ptr<ProceduralMesh>& proceduralMesh, Mesh*& staticMesh)
{
	bool isSelected = dynamic_cast<PM*>(proceduralMesh.get());
	std::string name;
	if constexpr(std::is_same_v<PM, Grid>)
		name = "Grid";
	else if constexpr(std::is_same_v<PM, SierpinskiTriangle>)
		name = "Sierpinski Triangle";
	else if constexpr(std::is_same_v<PM, SierpinskiTetrahedon>)
		name = "Sierpinski Tetrahedon";
	else if constexpr(std::is_same_v<PM, SierpinskiCarpet>)
		name = "Sierpinski Carpet";
	else
		static_assert(false);
	if(ImGui::Selectable(name.data(), &isSelected))
	{
		staticMesh = nullptr;
		proceduralMesh = std::make_unique<PM>();
	}
	if(isSelected)
		ImGui::SetItemDefaultFocus();
}
void Prop::drawUI()
{
	Transformed<Translation, Rotation, Scale>::drawUI();
	ImGui::BeginChild("###Prop", {ImGui::GetTextLineHeightWithSpacing() * 22, 0});
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Mesh");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	std::string name = getMesh().name.get();
	if(proceduralMesh)
		name = "Procedural";//TODO
	if(ImGui::BeginCombo("###Mesh", name.data()))
	{
		int id = 0;
		//static meshes
		for(auto& m : res::meshes::getAll())
		{
			ImGui::PushID(id++);
			bool isSelected = staticMesh == m.get();
			if(ImGui::Selectable(m->name.get().data(), &isSelected))
			{
				staticMesh = m.get();
				proceduralMesh = nullptr;
			}
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		//procedural meshes
		ImGui::Separator();
		addProceduralMeshItem<Grid>(proceduralMesh, staticMesh);
		addProceduralMeshItem<SierpinskiTriangle>(proceduralMesh, staticMesh);
		addProceduralMeshItem<SierpinskiTetrahedon>(proceduralMesh, staticMesh);
		addProceduralMeshItem<SierpinskiCarpet>(proceduralMesh, staticMesh);
		ImGui::EndCombo();
	}
	if(proceduralMesh)
		proceduralMesh->drawUI();
	ImGui::EndChild();
}

