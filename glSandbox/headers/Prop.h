#pragma once
#include "Node.h"
#include "Util.h"
#include "Mesh.h"
#include "Resources.h"
#include <optional>

class Prop : public Node
{
private:
	std::shared_ptr<Mesh> mesh = res::meshes::quad();

public:
	Name<Prop> name{"prop"};

public:
	Prop() = default;
	Prop(std::shared_ptr<Mesh> const& mesh);
	Prop(std::shared_ptr<Mesh>&& mesh);
	virtual ~Prop() = default;

public:
	Mesh& getMesh() const;
	Bounds getBounds() const override;
	//void drawUI() override;

};