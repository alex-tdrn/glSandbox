#include "Prop.h"
#include "Resources.h"

Prop::Prop(std::optional<unsigned int> meshIndex)
	:meshIndex(meshIndex)
{

}

std::optional<unsigned int>const& Prop::getMeshIndex() const
{
	return meshIndex;
}

Bounds Prop::getBounds() const
{
	Bounds bounds;
	if(meshIndex)
		bounds = resources::meshes[*meshIndex].getBounds();
	auto const& t = getTransformation();
	return bounds * t;
}
