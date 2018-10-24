#include "CubemapManager.h"
#include "CubemapRenderer.h"
#include "UIUtilities.h"
#include "MeshManager.h"

#include <glad/glad.h>
#include <imgui.h>
#include <memory>

Cubemap::Cubemap(unsigned int format, int width, int height,
	unsigned int pixelTransfer, unsigned int dataType)
	: format(format), width(width), height(height), 
	pixelTransfer(pixelTransfer), dataType(dataType)
{
	linear = false;
	mipmapping = false;
	allocate();
}

Cubemap::Cubemap(std::array<Texture, 6>&& faces)
	:faces(std::move(faces))
{
	
}

Cubemap::Cubemap(Texture&& equirectangularMap)
	:equirectangularMap(std::move(equirectangularMap))
{
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &ID);
}

void Cubemap::allocate() const
{
	allocated = true;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for(int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
			width, height, 0, pixelTransfer, dataType, nullptr);
	}
	if(mipmapping)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Cubemap::load() const
{
	if(faces)
	{
		faces->front().load();
		width = faces->front().width;
		height = faces->front().height;
		nrChannels = faces->front().nrChannels;
		format = faces->front().format;
		pixelTransfer = faces->front().pixelTransfer;
		dataType = faces->front().dataType;
		mipmapping = faces->front().mipmapping;
		linear = faces->front().linear;
		allocate();
		for(int i = 0; i < faces->size(); i++)
		{
			glCopyImageSubData((*faces)[i].getID(), GL_TEXTURE_2D, 0, 0, 0, 0,
				ID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, i, width, height, 1);
		}
	}
	else if(equirectangularMap)
	{
		equirectangularMap->load();
		height = equirectangularMap->height;
		width = height;
		nrChannels = equirectangularMap->nrChannels;
		format = equirectangularMap->format;
		pixelTransfer = equirectangularMap->pixelTransfer;
		dataType = equirectangularMap->dataType;
		mipmapping = equirectangularMap->mipmapping;
		linear = equirectangularMap->linear;
		allocate();

		glViewport(0, 0, width, height);
		unsigned int captureFBO;
		glGenFramebuffers(1, &captureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		//glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ID, 0);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		std::array<glm::mat4, 6> views = {
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{+1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, +1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, +1.0f}),
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}),
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, 0.0f, +1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
			projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f})
		};
		ShaderManager::equirectangularToCubemap()->use();
		for(int i = 0; i < 6; i++)
			ShaderManager::equirectangularToCubemap()->set(
			"views[" + std::to_string(i) + "]", views[i]);
		ShaderManager::equirectangularToCubemap()->set("equirectangularMap", 0);
		equirectangularMap->use(0);
		MeshManager::box()->use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
	}
	else
	{
		assert(false);
	}
}

Cubemap* Cubemap::convolute() const
{
	if(convolutedMap)
		return convolutedMap;
	if(!allocated)
		load();
	auto convoluted = std::make_unique<Cubemap>(format, 32, 32, pixelTransfer, dataType);
	convoluted->mipmapping = mipmapping;
	convoluted->linear = linear;
	convoluted->allocate();

	glViewport(0, 0, 32, 32);
	unsigned int captureFBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	//glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, convoluted->ID, 0);
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::array<glm::mat4, 6> views = {
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{+1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, +1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, +1.0f}),
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}),
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, 0.0f, +1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}),
		projection * glm::lookAt(glm::vec3(.0), glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f})
	};
	ShaderManager::convoluteCubemap()->use();
	for(int i = 0; i < 6; i++)
		ShaderManager::convoluteCubemap()->set(
		"views[" + std::to_string(i) + "]", views[i]);
	ShaderManager::convoluteCubemap()->set("sourceMap", 0);
	use(0);
	MeshManager::box()->use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &captureFBO);
	convolutedMap = CubemapManager::add(std::move(convoluted));
	return convolutedMap;
}

unsigned int Cubemap::getID() const
{
	if(!allocated)
		load();
	return ID;
}

void Cubemap::use(int location) const
{
	if(!allocated)
		load();
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	
}

void Cubemap::drawUI()
{
	IDGuard idGuard{this};
	if(!allocated)
		load();
	ImGui::Text("ID: %i", ID);
	OnDemandRenderer<CubemapRenderer>::drawUI(this);
	if(ImGui::Button("Convolute"))
	{
		if(!convolutedMap)
			convolute();
	}
}