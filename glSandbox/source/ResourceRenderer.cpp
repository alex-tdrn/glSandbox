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

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);
#ifndef NDEBUG
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ResourceRenderer::~ResourceRenderer()
{
	glDeleteTextures(1, &colorbuffer);
	glDeleteFramebuffers(1, &framebuffer);
}

ResourceRenderer::ResourceRenderer(ResourceRenderer&& other)
	:width(other.width), height(other.height)
{
	std::swap(this->framebuffer, other.framebuffer);
	std::swap(this->colorbuffer, other.colorbuffer);
}

ResourceRenderer& ResourceRenderer::operator=(ResourceRenderer&& other)
{
	this->width = other.width;
	this->height = other.height;
	std::swap(this->framebuffer, other.framebuffer);
	std::swap(this->colorbuffer, other.colorbuffer);
	return *this;
}

void ResourceRenderer::initializeRenderState()
{
	glViewport(0, 0, width, height);
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.0);
	glPointSize(1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void ResourceRenderer::drawUI()
{
	IDGuard idGuard{this};
	drawImage(colorbuffer, width, height);
	drawSettings();
}

