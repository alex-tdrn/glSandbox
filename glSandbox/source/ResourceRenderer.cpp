#include "ResourceRenderer.h"
#include "glad/glad.h"
#include "imgui.h"
#include "UIUtilities.h"

#include <glm/glm.hpp>
#include <algorithm>

ResourceRenderer::ResourceRenderer(int width, int height)
	:width(width), height(height)
{
	glGenTextures(1, &colorbuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
#ifndef NDEBUG
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ResourceRenderer::~ResourceRenderer()
{
	glDeleteTextures(1, &colorbuffer);
	glDeleteRenderbuffers(1, &renderbuffer);
	glDeleteFramebuffers(1, &framebuffer);
}

ResourceRenderer::ResourceRenderer(ResourceRenderer&& other)
	:width(other.width), height(other.height)
{
	std::swap(this->framebuffer, other.framebuffer);
	std::swap(this->colorbuffer, other.colorbuffer);
	std::swap(this->renderbuffer, other.renderbuffer);
}

ResourceRenderer& ResourceRenderer::operator=(ResourceRenderer&& other)
{
	this->width = other.width;
	this->height = other.height;
	std::swap(this->framebuffer, other.framebuffer);
	std::swap(this->colorbuffer, other.colorbuffer);
	std::swap(this->renderbuffer, other.renderbuffer);
	return *this;
}

void ResourceRenderer::initializeRenderState()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ResourceRenderer::drawUI()
{
	IDGuard idGuard{this};
	drawImage(colorbuffer, width, height, flipX, flipY);
	ImGui::Text("Flip");
	ImGui::SameLine();
	ImGui::Checkbox("X", &flipX);
	ImGui::SameLine();
	ImGui::Checkbox("Y", &flipY);
	drawSettings();
}

