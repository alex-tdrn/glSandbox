#pragma once
#include "ProceduralMesh.h"

class SierpinskiTetrahedon final : public ProceduralMesh
{
private:
	int iterations = 0;

public:
	SierpinskiTetrahedon() = default;
	SierpinskiTetrahedon(int iterations);

private:
	void generate() const override;

public:
	void drawUI() override;

};
