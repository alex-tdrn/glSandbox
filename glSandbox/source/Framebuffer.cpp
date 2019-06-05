#include "Framebuffer.h"

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
}

Framebuffer::Framebuffer(Framebuffer&&)
{
	//TODO
}

Framebuffer& Framebuffer::operator=(Framebuffer&&)
{
	// TODO
	return *this;
}

void Framebuffer::bind()
{
}

unsigned int Framebuffer::getColorbufferID() const
{
	return 0;
}
