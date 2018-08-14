#pragma once
#include "Node.h"
#include "Util.h"
#include "Mesh.h"

#include <optional>

class Prop : public Node
{
private:
	bool enabled = true;
	bool outlined = false;
	//TODO^^^
	std::shared_ptr<Mesh> mesh;

public:
	Name<Prop> name{"prop"};

public:
	Prop(std::shared_ptr<Mesh> const& mesh);
	Prop(std::shared_ptr<Mesh>&& mesh);
	virtual ~Prop() = default;

public:
	Mesh& getMesh() const;
	Bounds getBounds() const override;
	//void drawUI() override;

};