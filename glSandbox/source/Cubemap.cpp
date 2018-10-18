#include "Cubemap.h"
#include "CubemapRenderer.h"
#include "UIUtilities.h"

#include <glad/glad.h>
#include <imgui.h>

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
	if(!faces)
		assert(false);
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
}