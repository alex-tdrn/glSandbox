#include "Cubemap.h"
#include "Util.h"

#include <stb_image.h>
#include <glad/glad.h>
#include <imgui.h>

Cubemap::Cubemap(std::string name, std::array<std::string, 6> paths)
	: name(name), paths(std::move(paths))
{

}

std::string_view Cubemap::getName() const
{
	return name;
}

void Cubemap::use()
{
	if(!initialized)
		initialize();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	
}

void Cubemap::initialize()
{
	initialized = true;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	for(int i = 0; i < paths.size(); i++)
	{
		unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, STBI_default);
		if(data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
			throw "wtf";
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool Cubemap::drawUI()
{
	IDGuard idGuard{this};
	bool valueChanged = false;
	if(!initialized)
	{
		ImGui::Text("Loading...");
		return false;
	}
	/*if(drag2("UV Offset", 0.0001f, uvOffset.x, uvOffset.y, 0.0f, 1.0f))
		valueChanged = true;
	if(ImGui::SliderFloat("U ", &uvOffset.x, 0.0f, 1.0f))
		valueChanged = true;
	if(ImGui::SliderFloat("V ", &uvOffset.y, 0.0f, 1.0f))
		valueChanged = true;*/
	float const size = ImGui::GetContentRegionAvailWidth();
	ImGui::Text("%i x %i", width, height);
	ImGui::Text("# channels %i", nrChannels);
	//ImGui::Text("Path: %s", path.data());
	//ImGui::Image(ImTextureID(ID), ImVec2(size, size));// , ImVec2(-uvOffset.x, uvOffset.y), ImVec2(-uvOffset.x + 1.0f, uvOffset.y + 1.0f));
	if(false && size < 256 && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image(reinterpret_cast<ImTextureID>(ID), ImVec2(512, 512));
		ImGui::EndTooltip();
	}
	return valueChanged;
}