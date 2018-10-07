#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"
#include "Util.h"

#include <stb_image.h>
#include <imgui.h>

Texture::Texture(std::string const& path, bool linear)
	:path(path), linear(linear)
{
}

void Texture::load() const
{
	loaded = true;
	std::uint8_t* imageData = stbi_load(path->data(), &width, &height, &nrChannels, STBI_default);
	if(!imageData)
		throw "Could not load image from disk";
	GLenum format = 0;
	GLenum pixelTransfer = 0;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, pixelTransfer, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);
}

void Texture::use(int location) const
{
	if(!loaded)
	{
		if(!path)
			assert(false);
		load();
	}
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::drawUI()
{
	IDGuard idGuard{this};
	ImGui::BeginChild("###Texture");
	if(!loaded)
		load();
	
	float const size = std::min(ImGui::GetContentRegionAvailWidth(), 512.0f);
	ImGui::Text("Encoding: %S", linear ? "Linear" : "Gamma");
	ImGui::Text("ID %i", ID);
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

	ImGui::EndChild();
}