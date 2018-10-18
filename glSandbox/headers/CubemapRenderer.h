#include "ResourceRenderer.h"
#include "Cubemap.h"

class CubemapRenderer : public ResourceRenderer
{
public:
	using ResourceType = Cubemap;

private:
	Cubemap const* const cubemap;
	static inline float fov = 90.0f;
	static inline bool drawEdges = true;
	static inline glm::vec3 edgeColor{0.0f};

public:
	CubemapRenderer() = delete;
	CubemapRenderer(Cubemap*);
	CubemapRenderer(CubemapRenderer const&) = delete;
	CubemapRenderer(CubemapRenderer&&) = default;
	~CubemapRenderer() = default;
	CubemapRenderer& operator=(CubemapRenderer const&) = delete;
	CubemapRenderer& operator=(CubemapRenderer&&) = default;

private:
	void drawSettings() override;

public:
	void render() override;

};