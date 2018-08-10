#include "Mesh.h"
#include "Material.h"
#include "Util.h"
#include "Resources.h"

#include <imgui.h>
#include <array>

Mesh::Mesh(GLenum drawMode, Attributes&& attributes, std::optional<IndexBuffer>&& indices)
	: Asset<Mesh>("mesh"), drawMode(drawMode), vertexCount(attributes.array[AttributeType::positions]->size / attributes.array[AttributeType::positions]->stride),
	indexCount(indices? indices->count : 0), indexDataType(indices? indices->dataType : 0), indexedDrawing(indices)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if(attributes.interleaved)
	{
		glBufferData(GL_ARRAY_BUFFER, attributes.size, attributes.data, GL_STATIC_DRAW);

		for(auto const& attribute : attributes.array)
		{
			if(!attribute)
				continue;
			glEnableVertexAttribArray(attribute->attributeType);
			glVertexAttribPointer(attribute->attributeType, attribute->componentSize, attribute->dataType, GL_FALSE, attribute->stride, (void*) (attribute->offset));
		}
	}
	else
	{
		int bufferSize = 0;
		for(auto const& attribute : attributes.array)
			bufferSize += attribute ? attribute->size : 0;
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		int offset = 0;
		for(auto const& attribute : attributes.array)
		{
			if(!attribute)
				continue;
			glBufferSubData(GL_ARRAY_BUFFER, offset, attribute->size, attribute->data);
			glEnableVertexAttribArray(attribute->attributeType);
			glVertexAttribPointer(attribute->attributeType, attribute->componentSize, attribute->dataType, GL_FALSE, attribute->stride, (void*) (offset));
			offset += attribute->size;
		}
	}

	if(indices)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size, indices->data, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other)
	:Asset<Mesh>("mesh"), drawMode(other.drawMode), vertexCount(other.vertexCount),
	indexCount(other.indexCount), indexDataType(other.indexDataType), indexedDrawing(other.indexedDrawing),
	VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
{
	other.releaseResources = false;
}
Mesh::~Mesh()
{
	if(!releaseResources)
		return;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::use() const
{
	glBindVertexArray(VAO);
	if(indexedDrawing)
		glDrawElements(drawMode, indexCount, indexDataType, 0);
	else
		glDrawArrays(drawMode, 0, vertexCount);
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