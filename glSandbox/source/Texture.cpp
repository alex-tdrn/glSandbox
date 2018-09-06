#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"
#include "Util.h"

#include <stb_image.h>
#include <imgui.h>
#include <chrono>

Texture::Texture(std::string const path, bool linear)
	:path(path), linear(linear)
{
	loader = std::async(std::launch::async, [=]() -> Texture::ImageData {
		ImageData ret;
		ret.data = stbi_load(path.data(), &ret.width, &ret.height, &ret.nrChannels, STBI_default);
		if(!ret.data)
			throw("Could not load texture");
		return ret;
	});

}

glm::vec2 const Texture::getUVOffset() const
{
	return {-uvOffset.x, uvOffset.y};
}

bool Texture::isLoaded() const
{
	return loader.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void Texture::initialize(int location) const
{
	initialized = true;
	image = loader.get();
	GLenum format;
	GLenum pixelTransfer;
	switch(image.nrChannels)
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
	}
	if(!linear)
	{
		switch(image.nrChannels)
		{
			case 3:
				format = GL_SRGB8;
				break;
			case 4:
				format = GL_SRGB8_ALPHA8;
				break;
		}
	}
	glActiveTexture(GL_TEXTURE0 + location);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, pixelTransfer, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image.data);
	//resources::scenes::getActiveScene().update();
}

std::string_view Texture::getPath() const
{
	return path;
}

void Texture::use(int location) const
{
	if(!initialized)
	{
		if(isLoaded())
			initialize(location);
		else
		{
			if(this != &res::textures::placeholder())
				res::textures::placeholder().use(location);
			return;
		}
	}
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

bool Texture::drawUI()
{
	IDGuard idGuard{this};
	ImGui::Begin("tex");
	bool valueChanged = false;
	if(!initialized)
	{
		ImGui::Text("Loading...");
		ImGui::End();
		return false;
	}
	valueChanged |= drag2("UV Offset", 0.0001f, uvOffset.x, uvOffset.y, 0.0f, 1.0f);
	valueChanged |= ImGui::SliderFloat("U ", &uvOffset.x, 0.0f, 1.0f);
	valueChanged |= ImGui::SliderFloat("V ", &uvOffset.y, 0.0f, 1.0f);
	
	float const size = 512;
	ImGui::Text("%i x %i", image.width, image.height);
	ImGui::Text("# channels %i", image.nrChannels);
	ImGui::Text("Path: %s", path.data());
	ImGui::Image(ImTextureID(ID), ImVec2(size, size), ImVec2(-uvOffset.x, uvOffset.y), ImVec2(-uvOffset.x + 1.0f, uvOffset.y + 1.0f));
	if(false && size < 256 && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image(ImTextureID(ID), ImVec2(512, 512));
		ImGui::EndTooltip();
	}
	ImGui::End();
	return valueChanged;
}