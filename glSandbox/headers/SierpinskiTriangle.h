#pragma once
#include "ProceduralMesh.h"

class SierpinskiTriangle final : public ProceduralMesh
{
private:
	int iterations = 0;

public:
	SierpinskiTriangle() = default;
	SierpinskiTriangle(int iterations);

private:
	void update() const override;

public:
	void drawUI();

}; 
