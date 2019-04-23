#pragma once
#include <unordered_map>

class ResourceRenderer
{
private:
	unsigned int framebuffer = 0;
	unsigned int colorbuffer = 0;
	unsigned int renderbuffer = 0;
	int width = 0;
	int height = 0;
	bool outDated = true;
	static inline bool flipX = false;
	static inline bool flipY = false;

protected:
	ResourceRenderer() = delete;
	ResourceRenderer(int width, int height);
	virtual ~ResourceRenderer();
	ResourceRenderer(ResourceRenderer const&) = delete;
	ResourceRenderer(ResourceRenderer&&);
	ResourceRenderer& operator=(ResourceRenderer const&) = delete;
	ResourceRenderer& operator=(ResourceRenderer&&);

protected:
	void initializeRenderState();
	virtual void render() = 0;
	virtual void drawSettings() = 0;

public:
	void drawUI();

};

template <typename Renderer>
class OnDemandRenderer
{
	//TODO: add cleanup of unused renderers, to free up gpu memory
private:
	static inline std::unordered_map<typename Renderer::ResourceType*, Renderer> renderers;

public:
	static void drawUI(typename Renderer::ResourceType* resource)
	{
		Renderer& renderer = renderers.try_emplace(resource, resource).first->second;
		renderer.render();
		renderer.drawUI();
	}
};
