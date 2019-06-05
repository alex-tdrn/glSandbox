#pragma once

class Framebuffer
{
private:
	unsigned int msaaFramebufferID = 0;
	unsigned int msaaColorbufferID = 0;
	unsigned int msaaRenderbufferID = 0;
	unsigned int mainFramebufferID = 0;
	unsigned int mainColorbufferID = 0;
	unsigned int mainRenderbufferID = 0;
	int samples = 0;
	int width = 100;
	int height = 100;

public:
	Framebuffer();
	~Framebuffer();
	Framebuffer(Framebuffer const&) = delete;
	Framebuffer(Framebuffer&&);
	Framebuffer& operator=(Framebuffer const&) = delete;
	Framebuffer& operator=(Framebuffer&&);

public:
	void bind();
	unsigned int getColorbufferID() const;
};

