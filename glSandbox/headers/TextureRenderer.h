#include "ResourceRenderer.h"

class Texture;

class TextureRenderer : public ResourceRenderer
{
public:
	using ResourceType = Texture;

private:
	Texture const* const texture;
	static inline bool visualizeChannel[4] = {
		true, true, true, true
	};
	static inline bool linearize = false;
	static inline float nearPlane = 1.0f;
	static inline float farPlane = 100.0f;

public:
	TextureRenderer() = delete;
	TextureRenderer(Texture*);
	TextureRenderer(TextureRenderer const&) = delete;
	TextureRenderer(TextureRenderer&&) = default;
	~TextureRenderer() = default;
	TextureRenderer& operator=(TextureRenderer const&) = delete;
	TextureRenderer& operator=(TextureRenderer&&) = default;

private:
	void drawSettings() override;

public:
	void render() override;
	
};