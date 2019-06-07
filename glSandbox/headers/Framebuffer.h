#pragma once
#include <glm/glm.hpp>

class Framebuffer
{
private:
	unsigned int framebufferID = 0;
	unsigned int colorbufferID = 0;
	unsigned int renderbufferID = 0;
	glm::ivec2 size = { 800, 600 };

public:
	Framebuffer();
	~Framebuffer();
	Framebuffer(Framebuffer const&) = delete;
	Framebuffer(Framebuffer&&) = delete;
	Framebuffer& operator=(Framebuffer const&) = delete;
	Framebuffer& operator=(Framebuffer&&) = delete;

private:
	void update();

public:
	void resize(glm::ivec2 size);
	void bind();
	unsigned int getColorbufferID() const;
	virtual void drawUI() const;
};

