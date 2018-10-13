#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"
#include "Util.h"

#include <stb_image.h>
#include <imgui.h>
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
	name.set(filename.filename().string());
}

Texture::~Texture()
{
	//TODO
}

void Texture::allocate(void* imageData) const
{
	allocated = true;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, pixelTransfer, dataType, imageData);
	if(mipmapping)
		glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::load() const
{
	if(!path)
		assert(false);
	
	std::uint8_t* imageData = stbi_load(path->data(), &width, &height, &nrChannels, STBI_default);
	if(!imageData)
		throw "Could not load image from disk";
	dataType = GL_UNSIGNED_BYTE;
	switch(nrChannels)
	{
		case 1:
			format = GL_R8;
			pixelTransfer = GL_RED;
			break;
		case 2:
			format = GL_RG8;
			pixelTransfer = GL_RG;
			break;
		case 3:
			format = GL_RGB8;
			pixelTransfer = GL_RGB;
			break;
		case 4:
			format = GL_RGBA8;
			pixelTransfer = GL_RGBA;
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
	allocate(imageData);
	stbi_image_free(imageData);
}

unsigned int Texture::getID() const
{
	return ID;
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
	ImGui::Text("Encoding: %s", linear ? "Linear" : "Gamma");
	ImGui::Text("Format: ");
	ImGui::SameLine();
	switch(format)
	{
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
		default:
			assert(false);
	}
	ImGui::Text("%i x %i", width, height);
	ImGui::Text("# channels %i", nrChannels);
	if(path)
		ImGui::Text("Path: %s", path->data());

	float const size = std::min(ImGui::GetContentRegionAvailWidth(), 512.0f);
	bool flipY = !path;
	ImVec2 uv1{0.0, flipY ? 1.0f : 0.0f};
	ImVec2 uv2{1.0, flipY ? 0.0f : 1.0f};
	if(ImGui::ImageButton(ImTextureID(ID), ImVec2(size, size), uv1, uv2))
		ImGui::OpenPopup(name.get().data());
	if(ImGui::BeginPopupModal(name.get().data(), nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar))
	{
		float const percentOfScreen = 0.9f;
		glm::vec2 scale{width / info::windowWidth, height / info::windowHeight};
		glm::vec2 imageSize;
		if(scale.x > scale.y)//stretch by width
		{
			imageSize.x = info::windowWidth * percentOfScreen;
			imageSize.y = static_cast<float>(height) / width * imageSize.x;
		}
		else//stretch by height
		{
			imageSize.y = info::windowHeight * percentOfScreen;
			imageSize.x = static_cast<float>(width) / height * imageSize.y;
		}
		ImGui::BeginPopupModal(name.get().data());
		ImGui::Image(ImTextureID(ID), ImVec2(imageSize.x, imageSize.y), uv1, uv2);
		if(ImGui::IsAnyItemActive())
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}