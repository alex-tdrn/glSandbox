#include "SierpinskiTetrahedon.h"
#include "Mesh.h"

#include <vector>

SierpinskiTetrahedon::SierpinskiTetrahedon(int iterations)
	:iterations(iterations)
{
}

void SierpinskiTetrahedon::generate() const
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	float const tHeight = 0.866025f;
	glm::vec3 A{glm::vec3{+0.0f, 2 * tHeight / 3, 0.0f}};
	glm::vec3 B{glm::vec3{-0.5f, - tHeight / 3, tHeight / 3}};
	glm::vec3 C{glm::vec3{+0.5f, - tHeight / 3, tHeight / 3}};
	glm::vec3 D{glm::vec3{0.0f, -tHeight / 3, - 2 * tHeight / 3}};
	glm::vec3 normals[4] = 
	{
		glm::cross(B - A, C - A),
		glm::cross(C - A, D - A),
		glm::cross(D - A, B - A),
		glm::cross(D - C, D - B)
	};
	auto addTri = [&](auto& A, auto& B, auto& C, int faceId) -> void{
		vertices.push_back({A, normals[faceId], glm::vec2{0.5, 1.0f}});
		vertices.push_back({B, normals[faceId], glm::vec2{0.0f, 0.0f}});
		vertices.push_back({C, normals[faceId], glm::vec2{1.0f, 0.0f}});
		indices.push_back(vertices.size() - 3);
		indices.push_back(vertices.size() - 2);
		indices.push_back(vertices.size() - 1);
	};
	auto processTet = [&indices, &addTri, &vertices](auto& A, auto& B, auto& C, auto& D, int iteration, auto& processTet) mutable -> void{
		if(iteration == 0)
		{
			addTri(A, B, C, 0);
			addTri(A, C, D, 1);
			addTri(A, D, B, 2);
			addTri(D, C, B, 3);
			return;
		}
		auto a{(A + B) / 2.0f};
		auto b{(B + C) / 2.0f};
		auto c{(C + A) / 2.0f};
		auto d{(C + D) / 2.0f};
		auto e{(D + A) / 2.0f};
		auto f{(B + D) / 2.0f};

		processTet(A, a, c, e, iteration - 1, processTet);
		processTet(a, B, b, f, iteration - 1, processTet);
		processTet(c, b, C, d, iteration - 1, processTet);
		processTet(e, f, d, D, iteration - 1, processTet);
	};


	processTet(A, B, C, D, iterations, processTet);
	auto bounds = calculateBounds(vertices);
	mesh = std::make_unique<Mesh>(bounds, GL_TRIANGLES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices)));
}

void SierpinskiTetrahedon::drawUI()
{
	ImGui::BeginGroup();
	ImGui::InputInt("Iterations ", &iterations, 1);
	if(iterations < 0)
		iterations = 0;
	ImGui::EndGroup();
	if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
		parametersChanged = true;
}
