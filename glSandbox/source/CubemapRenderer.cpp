#include "CubemapRenderer.h"
#include "ShaderManager.h"
#include "CubemapManager.h"
#include "MeshManager.h"
#include "imgui.h"

#include <GLFW/glfw3.h>

CubemapRenderer::CubemapRenderer(Cubemap* cubemap)
	: ResourceRenderer(1024, 1024), cubemap(cubemap)
{

}

void CubemapRenderer::drawSettings()
{
	ImGui::InputFloat("FOV", &fov, 1.0f, 5.0f);
	ImGui::Checkbox("Draw Edges", &drawEdges);
	if(drawEdges)
	{
		ImGui::SameLine();
		ImGui::ColorEdit3("###EdgesColor", &edgeColor.r, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs);
	}
}

void CubemapRenderer::render()
{
	initializeRenderState();
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glm::mat4 projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 2.0f);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(glfwGetTime()) * 15), glm::vec3{0.0f, 1.0f, 0.0f});

	ShaderManager::visualizeCubemap()->use();
	ShaderManager::visualizeCubemap()->set("projection", projection);
	ShaderManager::visualizeCubemap()->set("model", model);
	ShaderManager::visualizeCubemap()->set("cubemap", 0);
	cubemap->use(0);
	MeshManager::box()->use();
	if(drawEdges)
	{
		glDisable(GL_DEPTH_TEST);
		ShaderManager::visualizeMesh()->use();
		ShaderManager::visualizeMesh()->set("projection", projection);
		ShaderManager::visualizeMesh()->set("model", model);
		ShaderManager::visualizeMesh()->set("ndcTransform", glm::mat4(1.0f));
		ShaderManager::visualizeMesh()->set("visualizeAttribute", -1);
		ShaderManager::visualizeMesh()->set("color", edgeColor);
		MeshManager::boxWireframe()->use();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
