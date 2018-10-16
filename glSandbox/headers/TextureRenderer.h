#include "ResourceRenderer.h"

class Texture;

class TextureRenderer : public ResourceRenderer
{
public:
	using ResourceType = Texture;

private:
	Texture const* const texture;
	bool visualizeChannel[4] = {
		true, true, true, true
	};

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