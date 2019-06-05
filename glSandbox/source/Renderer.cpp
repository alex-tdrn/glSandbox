#include "Renderer.h"

Framebuffer const* Renderer::getResourceHandle() const
{
	return framebuffer.get();
}

std::string Renderer::getNamePrefix() const
{
	return "Renderer";
}

