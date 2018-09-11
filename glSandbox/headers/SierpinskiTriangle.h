#pragma once
#include "ProceduralMesh.h"

class SierpinskiTriangle final : public ProceduralMesh
{
private:
	int iterations = 1;

public:
	SierpinskiTriangle() = default;
	SierpinskiTriangle(int iterations);

private:
	void generate() const override;

public:
	void drawUI() override;

}; 
