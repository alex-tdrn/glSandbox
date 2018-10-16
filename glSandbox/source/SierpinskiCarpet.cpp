#include "SierpinskiCarpet.h"
#include "Mesh.h"
#include "imgui.h"


#include <vector>

SierpinskiCarpet::SierpinskiCarpet(int iterations)
	:iterations(iterations)
{
}

void SierpinskiCarpet::generate() const
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	auto addVertex = [&](glm::vec3&& P) -> int{
		vertices.push_back({P, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{P.x + 0.5f, P.y + 0.5f}});
		return vertices.size() - 1;
	};
	auto addQuad = [&](int A, int B, int C, int D){
		indices.push_back(A);
		indices.push_back(B);
		indices.push_back(C);
		indices.push_back(C);
		indices.push_back(D);
		indices.push_back(A);
	};
	auto processQuad = [&indices, &addVertex, &addQuad, &vertices](int A, int B, int C, int D, int iteration, auto& processQuad) mutable -> void{
		if(iteration == 0)
		{
			addQuad(A, B, C, D);
			return;
		}
		float const xLeftEdge = vertices[A].position.x;
		float const xRightEdge = vertices[D].position.x;
		float const yBottomEdge = vertices[B].position.y;
		float const yTopEdge = vertices[A].position.y;

		float const dist = std::abs(xLeftEdge - xRightEdge) / 3;

		float const xLeft = xLeftEdge + dist;
		float const xRight = xRightEdge - dist;
		float const yBottom = yBottomEdge + dist;
		float const yTop = yTopEdge - dist;

		int a = addVertex(glm::vec3{xLeft, yTop, 0.0f});
		int b = addVertex(glm::vec3{xLeft, yBottom, 0.0f});
		int c = addVertex(glm::vec3{xRight, yBottom, 0.0f});
		int d = addVertex(glm::vec3{xRight, yTop, 0.0f});

		int e = addVertex(glm::vec3{xLeftEdge, yTop, 0.0f});
		int f = addVertex(glm::vec3{xLeftEdge, yBottom, 0.0f});

		int g = addVertex(glm::vec3{xLeft, yBottomEdge, 0.0f});
		int h = addVertex(glm::vec3{xRight, yBottomEdge, 0.0f});

		int i = addVertex(glm::vec3{xRightEdge, yBottom, 0.0f});
		int j = addVertex(glm::vec3{xRightEdge, yTop, 0.0f});

		int k = addVertex(glm::vec3{xRight, yTopEdge, 0.0f});
		int l = addVertex(glm::vec3{xLeft, yTopEdge, 0.0f});

		if(iteration == 1)
		{
			addQuad(A, f, b, l);
			addQuad(f, B, h, c);
			addQuad(C, j, d, h);
			addQuad(D, l, a, j);
			return;
		}

		processQuad(A, e, a, l, iteration - 1, processQuad);
		processQuad(e, f, b, a, iteration - 1, processQuad);
		processQuad(f, B, g, b, iteration - 1, processQuad);
		processQuad(b, g, h, c, iteration - 1, processQuad);

		processQuad(c, h, C, i, iteration - 1, processQuad);
		processQuad(d, c, i, j, iteration - 1, processQuad);
		processQuad(k, d, j, D, iteration - 1, processQuad);
		processQuad(l, a, d, k, iteration - 1, processQuad);

	};
	addVertex({-0.5f, +0.5f, 0.0f});
	addVertex({-0.5f, -0.5f, 0.0f});
	addVertex({+0.5f, -0.5f, 0.0f});
	addVertex({+0.5f, +0.5f, 0.0f});
	processQuad(0, 1, 2, 3, iterations, processQuad);
	auto bounds = calculateBounds(vertices);
	mesh = std::make_unique<Mesh>(bounds, GL_TRIANGLES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices)));
}

void SierpinskiCarpet::drawUI()
{
	ImGui::BeginGroup();
	ImGui::InputInt("Iterations ", &iterations, 1);
	if(iterations < 0)
		iterations = 0;
	ImGui::EndGroup();
	if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
		parametersChanged = true;
}
