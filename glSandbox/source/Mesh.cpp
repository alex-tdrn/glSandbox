#include "Mesh.h"
#include "UIUtilities.h"

#include <imgui.h>

Mesh::Mesh(Bounds bounds, GLenum drawMode, Attributes&& attributes, std::optional<IndexBuffer>&& indices)
	: bounds(bounds), drawMode(drawMode), 
	vertexCount((attributes.interleaved ? attributes.size : attributes.array[AttributeType::positions]->size) / attributes.array[AttributeType::positions]->stride),
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
		size_t bufferSize = 0;
		for(auto const& attribute : attributes.array)
			bufferSize += attribute ? attribute->size : 0;
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		size_t offset = 0;
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
	: bounds(other.bounds), drawMode(other.drawMode), vertexCount(other.vertexCount),
	indexCount(other.indexCount), indexDataType(other.indexDataType), indexedDrawing(other.indexedDrawing),
	VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

Bounds const& Mesh::getBounds() const
{
	return bounds;
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

void Mesh::drawUI()
{
	IDGuard idGuard{this};

	ImGui::BeginChild("###Mesh");
	ImGui::Value("VAO", VAO);
	ImGui::SameLine();
	ImGui::Value("VBO", VBO);
	if(indexedDrawing)
	{
		ImGui::SameLine();
		ImGui::Value("EBO", EBO);
	}
	ImGui::Value("vertices", vertexCount);
	if(indexedDrawing)
	{
		ImGui::Value("indices", indexCount);
		ImGui::Text("Index Datatype");
		ImGui::SameLine();
		ImGui::Text(glEnumToString(indexDataType).data());
	}
	ImGui::Text("Draw Mode:");
	ImGui::SameLine();
	ImGui::Text(glEnumToString(drawMode).data());
	auto[bMin, bMax] = getBounds().getValues();
	ImGui::Text("Bounds (%.1f, %.1f, %.1f) - (%.1f, %.1f, %.1f)", bMin.x, bMin.y, bMin.z, bMax.x, bMax.y, bMax.z);
	ImGui::EndChild();
}

Mesh::Attributes buildAttributes(std::vector<SimpleVertex>&& vertices)
{
	Mesh::Attributes::AttributeBuffer positions;
	positions.attributeType = Mesh::AttributeType::positions;
	positions.componentSize = 3;
	positions.dataType = GL_FLOAT;
	positions.stride = sizeof(SimpleVertex);
	positions.offset = offsetof(SimpleVertex, position);

	Mesh::Attributes attributes;
	attributes.array[Mesh::AttributeType::positions] = positions;
	attributes.interleaved = true;
	attributes.data = reinterpret_cast<uint8_t const*>(vertices.data());
	attributes.size = vertices.size() * sizeof(SimpleVertex);
	return attributes;
}

Mesh::Attributes buildAttributes(std::vector<Vertex>&& vertices)
{
	Mesh::Attributes::AttributeBuffer positions;
	positions.attributeType = Mesh::AttributeType::positions;
	positions.componentSize = 3;
	positions.dataType = GL_FLOAT;
	positions.stride = sizeof(Vertex);
	positions.offset = offsetof(Vertex, position);

	Mesh::Attributes::AttributeBuffer normals;
	normals.attributeType = Mesh::AttributeType::normals;
	normals.componentSize = 3;
	normals.dataType = GL_FLOAT;
	normals.stride = sizeof(Vertex);
	normals.offset = offsetof(Vertex, normal);

	Mesh::Attributes::AttributeBuffer texcoords;
	texcoords.attributeType = Mesh::AttributeType::texcoords;
	texcoords.componentSize = 2;
	texcoords.dataType = GL_FLOAT;
	texcoords.stride = sizeof(Vertex);
	texcoords.offset = offsetof(Vertex, texcoords);

	Mesh::Attributes attributes;
	attributes.array[Mesh::AttributeType::positions] = positions;
	attributes.array[Mesh::AttributeType::normals] = normals;
	attributes.array[Mesh::AttributeType::texcoords] = texcoords;
	attributes.interleaved = true;
	attributes.data = reinterpret_cast<uint8_t const*>(vertices.data());
	attributes.size = vertices.size() * sizeof(Vertex);
	return attributes;
}