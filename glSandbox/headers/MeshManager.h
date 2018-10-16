#pragma once
#include "ResourceManager.h"
#include "Mesh.h"

class MeshManager : public ResourceManager<Mesh>
{
public:
	static void initialize();
	static Mesh* quad();
	static Mesh* box();
	static Mesh* boxWireframe();
};

