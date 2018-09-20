#pragma once
#include "ProceduralMesh.h"

class Grid final : public ProceduralMesh
{
private:
	int resolution = 1;

public:
	Grid() = default;
	Grid(int resolution);

private:
	void generate() const override;

public:
	void drawUI() override;

};