#include "SierpinskiTriangle.h"
#include "Mesh.h"

#include <vector>

SierpinskiTriangle::SierpinskiTriangle(int iterations)
	:iterations(iterations)
{
}

void SierpinskiTriangle::generate() const
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	float const tHeight = 0.866025f;
	auto addVertex = [&](glm::vec3&& P) -> int{
		vertices.push_back({P, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{P.x + 0.5f, (tHeight + P.y) / tHeight}});
		return vertices.size() - 1;
	};
	addVertex({0.0f, 2 * tHeight / 3, 0.0f});
	addVertex({-0.5f, - tHeight / 3, 0.0f});
	addVertex({0.5f, - tHeight / 3, 0.0f});
	auto processTriangle = [&indices, &addVertex, &vertices](int A, int B, int C, int iteration, auto& processTriangle) mutable -> void{
		if(iteration == 0)
		{
			indices.push_back(A);
			indices.push_back(B);
			indices.push_back(C);
			return;
		}
		int a = addVertex((vertices[B].position + vertices[C].position) / 2.0f);
		int b = addVertex((vertices[C].position + vertices[A].position) / 2.0f);
		int c = addVertex((vertices[A].position + vertices[B].position) / 2.0f);
		processTriangle(A, c, b, iteration - 1, processTriangle);
		processTriangle(c, B, a, iteration - 1, processTriangle);
		processTriangle(b, a, C, iteration - 1, processTriangle);
	};
	processTriangle(0, 1, 2, iterations, processTriangle);
	auto bounds = calculateBounds(vertices);
	mesh = std::make_unique<Mesh>(bounds, GL_TRIANGLES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices)));
}

void SierpinskiTriangle::drawUI()
{
	ImGui::BeginGroup();
	ImGui::InputInt("Iterations ", &iterations, 1);
	if(iterations < 0)
		iterations = 0;
	ImGui::EndGroup();
	if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
		parametersChanged = true;
}
