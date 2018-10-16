#include "Grid.h"
#include "Mesh.h"
#include "UIUtilities.h"

#include <vector>

Grid::Grid(int resolution)
	:resolution(resolution)
{
}

void Grid::generate() const
{
	std::vector<SimpleVertex> vertices;
	std::vector<uint16_t> indices;
	int const sideLength = resolution + 1;
	float const step = 2.0f / resolution;
	int idx = 0;
	for(int x = 0; x < sideLength; x++, idx += 2)
	{
		vertices.push_back({glm::vec3{-1.0f + x * step, 0.0f, -1.0f}});
		vertices.push_back({glm::vec3{-1.0f + x * step, 0.0f, +1.0f}});
		indices.push_back(idx);
		indices.push_back(idx + 1);
	}
	indices.push_back(0);
	indices.push_back((sideLength - 1) * 2);
	indices.push_back(1);
	indices.push_back(sideLength * 2 - 1);
	for(int z = 1; z < sideLength - 1; z++, idx += 2)
	{
		vertices.push_back({glm::vec3{-1.0f, 0.0f, -1.0f + z * step}});
		vertices.push_back({glm::vec3{+1.0f, 0.0f, -1.0f + z * step}});
		indices.push_back(idx);
		indices.push_back(idx + 1);
	}
	auto bounds = calculateBounds(vertices);
	mesh = std::make_unique<Mesh>(bounds, GL_LINES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices)));
}

void Grid::drawUI()
{
	ImGui::BeginGroup();
	ImGui::InputInt("Resolution", &resolution, 1);
	if(resolution < 1)
		resolution = 1;
	ImGui::EndGroup();
	if(ImGui::IsItemActive() || ImGui::IsItemDeactivatedAfterChange())
		parametersChanged = true;
}
