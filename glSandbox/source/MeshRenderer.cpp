#include "MeshRenderer.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "imgui.h"

MeshRenderer::MeshRenderer(Mesh* mesh)
	: ResourceRenderer(1024, 1024), mesh(mesh)
{

}

void MeshRenderer::drawSettings()
{
	auto drawAttributeOption = [&](std::string_view title, Mesh::AttributeType value){
		if(!mesh->hasAttribute(value))
			return;
		ImGui::SameLine();
		if(ImGui::RadioButton(title.data(), visualizeAttribute == value))
			visualizeAttribute = value;
	};
	ImGui::Text("Visualize Attributes ");
	drawAttributeOption("Positions", Mesh::AttributeType::positions);
	drawAttributeOption("Normals", Mesh::AttributeType::normals);
	drawAttributeOption("Tangents", Mesh::AttributeType::tangents);
	drawAttributeOption("Texture Coordinates", Mesh::AttributeType::texcoords);
}

void MeshRenderer::render()
{
	initializeRenderState();
	ShaderManager::visualizeMesh()->use();
	ShaderManager::visualizeMesh()->set("visualizeAttribute", static_cast<int>(visualizeAttribute));
	mesh->use();
}
