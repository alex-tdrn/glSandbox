#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"
#include "Util.h"

#include <stb_image.h>
#include <imgui.h>
#include <filesystem>

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

void Texture::load() const
{
	if(!path)
		assert(false);
	loaded = true;
	
	std::uint8_t* imageData = stbi_load(path->data(), &width, &height, &nrChannels, STBI_default);
	if(!imageData)
		throw "Could not load image from disk";
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
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, pixelTransfer, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);
}

void Texture::use(int location) const
{
	if(!loaded)
		load();
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::drawUI()
{
	IDGuard idGuard{this};
	if(!loaded)
		load();
	
	float const size = std::min(ImGui::GetContentRegionAvailWidth(), 512.0f);
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
	
	if(ImGui::ImageButton(ImTextureID(ID), ImVec2(size, size)))
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
			imageSize.x =  info::windowWidth * percentOfScreen;
			imageSize.y = static_cast<float>(height) / width * imageSize.x;
		}
		else//stretch by height
		{
			imageSize.y = info::windowHeight * percentOfScreen;
			imageSize.x = static_cast<float>(width) / height * imageSize.y;
		}
		ImGui::BeginPopupModal(name.get().data());
		ImGui::Image(ImTextureID(ID), ImVec2(imageSize.x, imageSize.y));
		if(ImGui::IsAnyItemActive())
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

}