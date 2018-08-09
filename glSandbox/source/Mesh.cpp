#include "Mesh.h"
#include "Material.h"
#include "Util.h"
#include "Resources.h"

#include <imgui.h>
#include <algorithm>

Mesh::Mesh(VertexBuffer&& vertices, IndicesBuffer&& indices, Material* material)
	: vertices(vertices), indices(indices), material(material), NamedAsset<Mesh>("mesh")
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int const bufferSize = vertices.positions.size + vertices.normals.size + vertices.textureCoords.size;
	glBufferData(GL_ARRAY_BUFFER, bufferSize , nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.positions.size, vertices.positions.data);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.positions.size, vertices.normals.size, vertices.normals.data);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.positions.size + vertices.normals.size, vertices.textureCoords.size, vertices.textureCoords.data);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices.positions.stride, (void*) 0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertices.normals.stride, (void*) vertices.positions.size);
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertices.textureCoords.stride, (void*) (vertices.normals.size + vertices.positions.size));

	if(indices.size)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size , indices.data, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}
Mesh::~Mesh()
{
	/*glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);*/
}

void Mesh::draw(Shader shader) const
{
	//if(material)
	//{
	//	material->use(shader);
	//}
	//else
	//{
	//	shader.set("material.hasDiffuseMap", false);
	//	shader.set("material.hasSpecularMap",false);
	//	shader.set("material.hasOpacityMap", false);
	//}
	///*else
	//{
	//	shader.set("material.hasDiffuseMap", true);
	//	shader.set("material.diffuseMap", 1);
	//	resources::textures::placeholder.use(1);
	//}*/
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size / 2, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

bool Mesh::drawUI()
{
	return false;
	//IDGuard idGuard{this};

	//bool valueChanged = false;
	//if(ImGui::TreeNode(name.data()))
	//{
	//	//ImGui::Text("Vertices %i", vertices.size());
	//	ImGui::Text("Indices %i", indices.size());
	//	ImGui::Text("Triangles %i", indices.size() / 3);
	//	valueChanged |= material->drawUI();
	//	ImGui::TreePop();
	//}
	//return valueChanged;
}