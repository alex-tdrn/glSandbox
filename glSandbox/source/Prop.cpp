#include "Prop.h"
#include "Resources.h"

Prop::Prop(std::optional<size_t> meshIndex)
	:meshIndex(meshIndex)
{

}

std::optional<size_t>const& Prop::getMeshIndex() const
{
	return meshIndex;
}

Bounds Prop::getBounds() const
{
	if(meshIndex)
	{
		Bounds bounds{resources::meshes[*meshIndex].getBounds()};
		auto const& t = getTransformation();
		return bounds * t;
	}
	return {};
}
