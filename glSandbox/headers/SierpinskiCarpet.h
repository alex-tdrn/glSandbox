#pragma once
#include "ProceduralMesh.h"

class SierpinskiCarpet final : public ProceduralMesh
{
private:
	int iterations = 0;

public:
	SierpinskiCarpet() = default;
	SierpinskiCarpet(int iterations);

private:
	void update() const override;

public:
	void drawUI();

};
