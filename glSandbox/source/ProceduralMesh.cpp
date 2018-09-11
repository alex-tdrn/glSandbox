#include "ProceduralMesh.h"

Mesh* ProceduralMesh::get() const
{
	if(parametersChanged)
	{
		generate();
		parametersChanged = false;
	}
	return mesh.get();
}