#pragma once
#include "TransformedNode.h"
#include "Util.h"
#include "Resources.h"
#include "ProceduralMesh.h"
#include <memory>

class Mesh;

class Prop final : public Transformed<Translation, Rotation, Scale>
{
private:
	Name<Prop> name{"prop"};
	Mesh* staticMesh = nullptr;
	std::unique_ptr<ProceduralMesh> proceduralMesh = nullptr;

public:
	Prop() = default;
	Prop(Mesh* mesh);
	Prop(std::unique_ptr<ProceduralMesh>&& mesh);
	virtual ~Prop() = default;

public:
	Mesh& getMesh() const;
	Bounds getBounds() const override;
	void setName(std::string const& name) override;
	std::string const& getName() const override;
	void drawUI() override;

};