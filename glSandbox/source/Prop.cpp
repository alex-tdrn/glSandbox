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

std::optional<Bounds> Prop::getBounds() const
{
	if(meshIndex)
	{
		Bounds bounds{resources::meshes[*meshIndex].getBounds()};
		auto const& t = getTransformation();
		return bounds * t;
	}
	return std::nullopt;
}
