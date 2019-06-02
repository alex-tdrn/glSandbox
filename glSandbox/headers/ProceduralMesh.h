#pragma once
#include "Producer.h"
#include <memory>

class Mesh;

class ProceduralMesh : public Producer<Mesh>
{
protected:
	mutable std::unique_ptr<Mesh> mesh;

public:
	ProceduralMesh() = default;
	virtual ~ProceduralMesh() = default;

private:
	Mesh const* getResourceHandle() const override
	{
		return mesh.get();
	}

};