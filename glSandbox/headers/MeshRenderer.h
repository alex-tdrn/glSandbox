#include "ResourceRenderer.h"
#include "Mesh.h"

class MeshRenderer : public ResourceRenderer
{
public:
	using ResourceType = Mesh;

private:
	Mesh const* const mesh;
	Mesh::AttributeType visualizeAttribute = Mesh::AttributeType::positions;

public:
	MeshRenderer() = delete;
	MeshRenderer(Mesh*);
	MeshRenderer(MeshRenderer const&) = delete;
	MeshRenderer(MeshRenderer&&) = default;
	~MeshRenderer() = default;
	MeshRenderer& operator=(MeshRenderer const&) = delete;
	MeshRenderer& operator=(MeshRenderer&&) = default;

private:
	void drawSettings() override;

public:
	void render() override;

};