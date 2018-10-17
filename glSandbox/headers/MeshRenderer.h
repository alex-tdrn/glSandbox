#include "ResourceRenderer.h"
#include "Mesh.h"

class MeshRenderer : public ResourceRenderer
{
public:
	using ResourceType = Mesh;

private:
	Mesh const* const mesh;
	static inline int visualizeAttribute = Mesh::AttributeType::positions;
	static inline bool faceCulling = false;
	static inline enum PolygonMode
	{
		triangles,
		lines,
		both
	} polygonMode = both;
	static inline glm::vec3 wireframeColor{0.0f};
	static inline float lineWidth = 3.0f;

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