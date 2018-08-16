#include "Prop.h"
#include "Resources.h"

Prop::Prop(std::shared_ptr<Mesh> const& mesh)
	:mesh(mesh)
{

}

Prop::Prop(std::shared_ptr<Mesh>&& mesh)
	:mesh(std::move(mesh))
{

}

Mesh& Prop::getMesh() const
{
	return *mesh;
}

Bounds Prop::getBounds() const
{
	return mesh->getBounds() * getTransformation();
}

std::string_view Prop::getName() const
{
	return name.get();
}


