#include "Globals.h"


static std::vector<std::unique_ptr<Renderer>> secondaryRenderers;

std::vector<std::unique_ptr<Renderer>> const & settings::getAllRenderers()
{
	return secondaryRenderers;
}
void settings::addRenderer(std::unique_ptr<Renderer>&& r)
{
	secondaryRenderers.emplace_back(std::move(r));
}