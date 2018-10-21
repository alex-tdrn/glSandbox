#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "UIUtilities.h"
#include "TextureRenderer.h"

#include <stb_image.h>
#include <filesystem>

Texture::Texture(unsigned int format, int width, int height, 
	unsigned int pixelTransfer, unsigned int dataType, void* imageData)
	: format(format), width(width), height(height), 
	dataType(dataType), pixelTransfer(pixelTransfer)

{
	mipmapping = false;
	allocate(imageData);
}

Texture::Texture(std::string const& path, bool linear)
	:path(path), linear(linear)
{
	std::filesystem::path filename = path;
	if(filename.extension() == "hdr")
	{
		hdr = true;
		linear = true;
	}
	name.set(filename.filename().string());
}

Texture::Texture(Texture&& other)
	: allocated(other.allocated), ID(other.ID),
	width(other.width), height(other.height),
	nrChannels(other.nrChannels), format(other.format),
	pixelTransfer(other.pixelTransfer), dataType(other.dataType),
	mipmapping(other.mipmapping), linear(other.linear), path(other.path)
{
	other.ID = 0;
}

Texture& Texture::operator=(Texture&& other)
{
	allocated = other.allocated;  ID = other.ID;
	width = other.width; height = other.height;
	nrChannels = other.nrChannels; format = other.format;
	pixelTransfer = other.pixelTransfer; dataType = other.dataType;
	mipmapping = other.mipmapping; linear = other.linear; path = other.path;

	std::swap(this->ID, other.ID);
	return *this;
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::allocate(void* imageData) const
{
	allocated = true;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, pixelTransfer, dataType, imageData);
	if(mipmapping)
		glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::load() const
{
	if(!path)
		assert(false);
	
	void* imageData = nullptr;
	if(hdr)
	{
		imageData = stbi_loadf(path->data(), &width, &height, &nrChannels, STBI_default);
		dataType = GL_FLOAT;
		switch(nrChannels)
		{
			case 1:
				format = GL_R16F;
				break;
			case 2:
				format = GL_RG16F;
				break;
			case 3:
				format = GL_RGB16F;
				break;
			case 4:
				format = GL_RGBA16F;
				break;
			default:
				assert(false);
		}
	}
	else
	{
		imageData = stbi_load(path->data(), &width, &height, &nrChannels, STBI_default);
		dataType = GL_UNSIGNED_BYTE;
		switch(nrChannels)
		{
			case 1:
				format = GL_R8;
				break;
			case 2:
				format = GL_RG8;
				break;
			case 3:
				format = GL_RGB8;
				break;
			case 4:
				format = GL_RGBA8;
				break;
			default:
				assert(false);
		}
		if(!linear)
		{
			switch(nrChannels)
			{
				case 3:
					format = GL_SRGB8;
					break;
				case 4:
					format = GL_SRGB8_ALPHA8;
					break;
				default:
					assert(false);
			}
		}
	}
	switch(nrChannels)
	{
		case 1:
			pixelTransfer = GL_RED;
			break;
		case 2:
			pixelTransfer = GL_RG;
			break;
		case 3:
			pixelTransfer = GL_RGB;
			break;
		case 4:
			pixelTransfer = GL_RGBA;
			break;
		default:
			assert(false);
	}
	if(!imageData)
		throw "Could not load image from disk";
	allocate(imageData);
	stbi_image_free(imageData);
}

unsigned int Texture::getID() const
{
	if(!allocated)
		load();
	return ID;
}

int Texture::getWidth() const
{
	if(!allocated)
		load();
	return width;
}

int Texture::getHeight() const
{
	if(!allocated)
		load();
	return height;
}

int Texture::getNumberOfChannels() const
{
	if(!allocated)
		load();
	return nrChannels;
}

void Texture::use(int location) const
{
	if(!allocated)
		load();
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

bool Texture::isLinear() const
{
	return linear;
}

void Texture::drawUI()
{
	IDGuard idGuard{this};
	if(!allocated)
		load();
	
	ImGui::Text("ID %i", ID);
	ImGui::Text("Encoding: %s, %s", linear ? "Linear" : "Gamma", hdr ? "HDR" : "SDR");
	ImGui::Text("Format: ");
	ImGui::SameLine();
	switch(format)
	{
		case GL_R16F:
			ImGui::Text("GL_R16F");
			break;
		case GL_RG16F:
			ImGui::Text("GL_RG16F");
			break;
		case GL_RGB16F:
			ImGui::Text("GL_RGB16F");
			break;
		case GL_RGBA16F:
			ImGui::Text("GL_RGB16F");
			break;
		case GL_R8:
			ImGui::Text("GL_R8");
			break;
		case GL_RG8:
			ImGui::Text("GL_RG8");
			break;
		case GL_RGB8:
			ImGui::Text("GL_RGB8");
			break;
		case GL_RGBA8:
			ImGui::Text("GL_RGB8");
			break;
		case GL_SRGB8:
			ImGui::Text("GL_SRGB8");
			break;
		case GL_SRGB8_ALPHA8:
			ImGui::Text("GL_SRGB8_ALPHA8");
			break;
		case GL_DEPTH_COMPONENT:
			ImGui::Text("GL_DEPTH_COMPONENT");
			break;
		default:
			assert(false);
	}
	ImGui::Text("Pixel Transfer: ");
	ImGui::SameLine();
	switch(pixelTransfer)
	{
		case GL_RED:
			ImGui::Text("GL_RED");
			break;
		case GL_RG:
			ImGui::Text("GL_RG");
			break;
		case GL_RGB:
			ImGui::Text("GL_RGB");
			break;
		case GL_RGBA:
			ImGui::Text("GL_RGBA");
			break;
		case GL_DEPTH_COMPONENT:
			ImGui::Text("GL_DEPTH_COMPONENT");
			break;
		default:
			assert(false);
	}
	ImGui::Text("%i x %i", width, height);
	ImGui::Text("# channels %i", nrChannels);
	if(path)
		ImGui::Text("Path: %s", path->data());
	OnDemandRenderer<TextureRenderer>::drawUI(this);
}