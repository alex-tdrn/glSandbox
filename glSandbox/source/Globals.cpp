#include "Globals.h"


static std::vector<std::unique_ptr<Renderer_Legacy>> secondaryRenderers;

std::vector<std::unique_ptr<Renderer_Legacy>> const & settings::getAllRenderers()
{
	return secondaryRenderers;
}
void settings::addRenderer(std::unique_ptr<Renderer_Legacy>&& r)
{
	secondaryRenderers.emplace_back(std::move(r));
}