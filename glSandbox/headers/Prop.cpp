#include "Prop.h"

Prop::Prop(std::optional<unsigned int> meshIndex)
	:meshIndex(meshIndex)
{

}

std::optional<unsigned int>const& Prop::getMeshIndex() const
{
	return meshIndex;
}