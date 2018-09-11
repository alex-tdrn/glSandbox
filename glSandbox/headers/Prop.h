#pragma once
#include "TransformedNode.h"
#include "Util.h"
#include "Mesh.h"
#include "Resources.h"
#include <optional>

class Prop : public Transformed<Translation, Rotation, Scale>
{
private:
	Name<Prop> name{"prop"};
	Mesh* mesh = res::meshes::box();

public:
	Prop() = default;
	Prop(Mesh* mesh);
	virtual ~Prop() = default;

public:
	Mesh& getMesh() const;
	Bounds getBounds() const override;
	void setName(std::string const& name) override;
	std::string const& getName() const override;
	void drawUI() override;

};