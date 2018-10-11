#include "Cubemap.h"
#include "Util.h"

#include <glad/glad.h>
#include <imgui.h>

Cubemap::Cubemap(std::array<Texture, 6>&& faces)
	:faces(std::move(faces))
{

}

void Cubemap::load()
{
	loaded = true;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ID);
	for(int i = 0; i < faces.size(); i++)
		faces[i].copy(ID, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::use(int location)
{
	if(!loaded)
		load();
	glActiveTexture(location);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	
}

void Cubemap::drawUI()
{
	IDGuard idGuard{this};
	if(!loaded)
		load();
	ImGui::Text("ID: %i", ID);
	ImGui::Text("Face +X");
	faces[0].drawUI();
	ImGui::Text("Face -X");
	faces[1].drawUI();
	ImGui::Text("Face +Y");
	faces[2].drawUI();
	ImGui::Text("Face -Y");
	faces[3].drawUI();
	ImGui::Text("Face +Z");
	faces[4].drawUI();
	ImGui::Text("Face -Z");
	faces[5].drawUI();
}