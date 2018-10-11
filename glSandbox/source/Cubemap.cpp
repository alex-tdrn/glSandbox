#include "Cubemap.h"
#include "Util.h"

#include <glad/glad.h>
#include <imgui.h>

Cubemap::Cubemap(std::array<Texture, 6>&& faces)
	:faces(std::move(faces))
{

}

void Cubemap::load() const
{
	loaded = true;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for(int i = 0; i < faces.size(); i++)
	{
		faces[i].load();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
			faces[i].format, faces[i].width, faces[i].height, 0, faces[i].pixelTransfer, GL_UNSIGNED_BYTE, nullptr);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	for(int i = 0; i < faces.size(); i++)
	{
		glCopyImageSubData(faces[i].ID, GL_TEXTURE_2D, 0, 0, 0, 0,
			ID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, i, faces[i].width, faces[i].height, 1);
	}
}

void Cubemap::use(int location) const
{
	if(!loaded)
		load();
	glActiveTexture(GL_TEXTURE0 + location);
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