#include "TextureRenderer.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "imgui.h"

TextureRenderer::TextureRenderer(Texture* texture)
	: ResourceRenderer(texture->getWidth(), texture->getHeight()), texture(texture)
{

}

void TextureRenderer::drawSettings()
{
	ImGui::Text("Visualize Channels ");
	ImGui::SameLine();
	ImGui::Checkbox("R", &visualizeChannel[0]);
	if(texture->getNumberOfChannels() == 1)
		return;
	ImGui::SameLine();
	ImGui::Checkbox("G", &visualizeChannel[1]);
	if(texture->getNumberOfChannels() == 2)
		return;
	ImGui::SameLine();
	ImGui::Checkbox("B", &visualizeChannel[2]);
	if(texture->getNumberOfChannels() == 3)
		return;
	ImGui::SameLine();
	ImGui::Checkbox("A", &visualizeChannel[3]);
}

void TextureRenderer::render()
{
	initializeRenderState();
	ShaderManager::visualizeTexture()->use();
	for(int i = 0; i < 4; i++)
		ShaderManager::visualizeTexture()->set("visualizeChannel[" + std::to_string(i) + "]", visualizeChannel[i]);
	ShaderManager::visualizeTexture()->set("linear", texture->isLinear());
	ShaderManager::visualizeTexture()->set("textureLocation", 0);
	texture->use(0);
	MeshManager::quad()->use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
