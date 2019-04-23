#include "MeshRenderer.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "imgui.h"

#include <GLFW/glfw3.h>

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
	ImGui::SameLine();
	if(visualizeAttribute != -1 && !mesh->hasAttribute(Mesh::AttributeType(visualizeAttribute)))
		visualizeAttribute = -1;
	if(ImGui::RadioButton("None", visualizeAttribute == -1))
		visualizeAttribute = -1;
	drawAttributeOption("Positions", Mesh::AttributeType::positions);
	drawAttributeOption("Normals", Mesh::AttributeType::normals);
	drawAttributeOption("Tangents", Mesh::AttributeType::tangents);
	drawAttributeOption("Texture Coordinates", Mesh::AttributeType::texcoords);
	ImGui::Checkbox("Face Culling", &faceCulling);
	ImGui::Text("Polygon Mode");
	ImGui::SameLine();
	if(ImGui::RadioButton("Triangles", polygonMode == triangles))
		polygonMode = triangles;
	ImGui::SameLine();
	if(ImGui::RadioButton("Lines", polygonMode == lines))
		polygonMode = lines;
	ImGui::SameLine();
	if(ImGui::RadioButton("Both", polygonMode == both))
		polygonMode = both;
	ImGui::ColorEdit3("Wireframe Color", &wireframeColor.r,
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_NoInputs);
	ImGui::Text("Line Width");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	ImGui::SliderFloat("###Width", &lineWidth, 0.1f, 32.0f);

}

void MeshRenderer::render()
{
	initializeRenderState();
	if(faceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 3.0f, 7.0f) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 ndcTransform = glm::inverse(mesh->getBounds().getTransformation(true));
	glm::mat4 modelMatrix = 
		glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(glfwGetTime()) * 15), glm::vec3{0.0f, 1.0f, 0.0f});
	ShaderManager::visualizeMesh()->use();
	ShaderManager::visualizeMesh()->set("projection", projection);
	ShaderManager::visualizeMesh()->set("model", modelMatrix);
	ShaderManager::visualizeMesh()->set("ndcTransform", ndcTransform);
	if(polygonMode != triangles || !mesh->hasSurface())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(lineWidth);
		ShaderManager::visualizeMesh()->set("visualizeAttribute", -1);
		ShaderManager::visualizeMesh()->set("color", wireframeColor);
		mesh->use();
	}
	if(polygonMode != lines && mesh->hasSurface())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ShaderManager::visualizeMesh()->set("visualizeAttribute", visualizeAttribute);
		mesh->use();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
