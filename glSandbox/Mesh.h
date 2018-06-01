#pragma once
#include "Shader.h"

#include <glm/glm.hpp>
#include <vector>

class Material;
struct Vertex
{
	glm::vec3 position{0.0f, 0.0f, 0.0f};
	glm::vec3 normal{0.0f, 0.0f, 0.0f};
	glm::vec2 texCoords{0.0f, 0.0f};
};

class Mesh
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::vec2 uvOffset{0, 0};
	Material* material;
	static int ct;
public:
	Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material* material, std::string const name = "Mesh#" + std::to_string(ct++));
	~Mesh();

public:
	std::vector<Vertex> const& getVertices() const;
	std::vector<unsigned int> const& getIndices() const;
	void draw(Shader shader) const;
	bool drawUI();

};