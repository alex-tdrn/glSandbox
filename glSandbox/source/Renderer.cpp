#include "Renderer.h"
#include "glad/glad.h"
#include <imgui.h>

std::string Renderer::getNamePrefix() const
{
	return "Renderer";
}

Framebuffer const* Renderer::getResourceHandle() const
{
	return framebuffer.get();
}

void Renderer::update() const
{
	framebuffer->resize(size);
	framebuffer->bind();
	glClearColor(background.r, background.g, background.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::drawUI(bool* open)
{
	ImGui::Begin(getName().data(), open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::Image(ImTextureID(framebuffer->getColorbufferID()), ImVec2(512, 512 / (16.0 / 9.0)), ImVec2(0, 1), ImVec2(1, 0));
	if(ImGui::ColorEdit3("Background", &background[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float))
		settingsTimestamp.update();

	ImGui::End();
}

