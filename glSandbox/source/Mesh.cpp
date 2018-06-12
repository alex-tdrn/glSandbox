#include "Mesh.h"
#include "Material.h"
#include "Util.h"

#include <imgui.h>
#include <algorithm>

int Mesh::ct = 0;

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material* material, std::string name)
	: name(name), vertices(vertices), indices(indices), material(material)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}
Mesh::~Mesh()
{
	/*glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);*/
}
std::vector<Vertex> const& Mesh::getVertices() const
{
	return vertices;
}
std::vector<unsigned int> const& Mesh::getIndices() const
{
	return indices;
}

void Mesh::draw(Shader shader) const
{
	if(material)
	{
		material->use(shader);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Mesh::drawUI()
{
	IDGuard idGuard{this};

	bool valueChanged = false;
	if(ImGui::TreeNode(name.data()))
	{
		ImGui::Text("Vertices %i", vertices.size());
		ImGui::Text("Indices %i", indices.size());
		ImGui::Text("Triangles %i", indices.size() / 3);
		
		if(material->drawUI())
			valueChanged = true;

		ImGui::TreePop();
	}
	return valueChanged;
}