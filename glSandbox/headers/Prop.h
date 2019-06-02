#pragma once
#include "TransformedNode.h"
#include "Util.h"
#include "MaterialManager.h"
#include "ProceduralMesh.h"
#include <memory>

class Mesh;
class Material;

class Prop final : public Transformed<Translation, Rotation, Scale>
{
private:
	Mesh* staticMesh = nullptr;
	std::unique_ptr<ProceduralMesh> proceduralMesh = nullptr;
	Material* material = MaterialManager::uvChecker();

public:
	Prop() = default;
	Prop(Mesh* mesh, Material* material = MaterialManager::uvChecker());
	Prop(std::unique_ptr<ProceduralMesh>&& mesh, Material* material = MaterialManager::uvChecker());
	~Prop() = default;

protected:
	std::string getNamePrefix() const override;

public:
	const Mesh& getMesh() const;
	Material* getMaterial() const;
	Bounds getBounds() const override;
	void drawUI() override;

};