#pragma once
#include <memory>

class Mesh;

class ProceduralMesh
{
protected:
	mutable std::unique_ptr<Mesh> mesh;
	mutable bool parametersChanged = true;

protected:
	virtual void generate() const = 0;

public:
	Mesh* get() const;
	virtual void drawUI() = 0;

};