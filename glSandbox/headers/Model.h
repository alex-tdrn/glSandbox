#pragma once
#include "Mesh.h"
#include "Material.h"
#include <string_view>
#include <memory>

struct aiScene;
struct aiMesh;
struct aiNode;

class Model
{
private:
	std::string name;
	std::string path;
	mutable std::vector<Mesh> meshes;
	mutable std::vector<std::unique_ptr<Material>> materials;
	mutable int nVertices = 0;
	mutable int nIndices = 0;
	mutable bool initialized = false;
	inline static int ct = 0;

public:
	Model(std::string_view const path = "../res/models/nanosuit/nanosuit.obj", std::string const name = "Model#" + std::to_string(ct++));
	Model(const Model&) = delete;
	Model(Model&&) = default;
	~Model() = default;
	Model& operator=(const Model&) = delete;
	Model& operator=(Model&&) = default;

private:
	void load() const;
	void processNode(aiNode* node, const aiScene* scene, std::string_view const dir) const;
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::string_view const dir) const;

public:
	std::string_view const getName() const;
	bool isInitialized() const;
	void draw(Shader shader) const;
	bool drawUI();

};