#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include <imgui.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(std::string_view const path, std::string const name)
	:name(name), path(path)
{
	
}

void Model::load() const
{
	initialized = true;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
		return;
	}
	processNode(scene->mRootNode, scene, path.substr(0, path.find_last_of('/')));
}

void Model::processNode(aiNode* node, const aiScene* scene, std::string_view const dir) const
{
	for(int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, dir));
	}
	for(int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene, dir);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, std::string_view const dir) const
{
	std::vector<Vertex> vertices(mesh->mNumVertices);
	for(int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices[i].position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertices[i].normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		if(mesh->mTextureCoords[0])
			vertices[i].texCoords = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
	}

	std::vector<unsigned int> indices(mesh->mNumFaces * 3);
	for(int i = 0; i < mesh->mNumFaces; i++)
		for(int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices[i * 3 + j] = mesh->mFaces[i].mIndices[j];

	Material* material = nullptr;
	if(mesh->mMaterialIndex >= 0)
	{
		auto getTexturePath = [&](aiTextureType type) -> std::optional<std::string>{
			aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
			aiString path;
			if(assimpMaterial->GetTextureCount(type))
				assimpMaterial->GetTexture(type, 0, &path);
			if(std::string_view(path.C_Str()) != "")
				return std::string(dir) + "/" + std::string(path.C_Str());
			return std::nullopt;
		};
		std::optional<std::string> diffusePath = getTexturePath(aiTextureType_DIFFUSE);
		std::optional<std::string> specularPath = getTexturePath(aiTextureType_SPECULAR);

		for(auto& loadedMaterial : materials)
		{
			if(diffusePath && loadedMaterial->contains(*diffusePath))
			{
				material = loadedMaterial.get();
				break;
			}
			if(specularPath && loadedMaterial->contains(*specularPath))
				material = loadedMaterial.get();
		}
		if(material == nullptr)
		{
			std::optional<Texture> diffuseTexture;
			if(diffusePath)
				diffuseTexture = Texture{std::move(*diffusePath)};
			std::optional<Texture> specularTexture;
			if(specularPath)
				specularTexture = Texture{std::move(*specularPath)};
			material = new Material(diffuseTexture, specularTexture);
			materials.emplace_back(material);
		}
	}
	nVertices += vertices.size();
	nIndices += indices.size();
	if(mesh->mName.length > 0)
		return Mesh(std::move(vertices), std::move(indices), material, mesh->mName.C_Str());
	else
		return Mesh(std::move(vertices), std::move(indices), material);
}

bool Model::isInitialized() const
{
	if(!initialized)
		return false;
	bool ret = true;
	for(auto& material : materials)
		ret = ret && material->isInitialized();
	return ret;
}

void Model::draw(Shader shader) const
{
	if(!initialized)
		load();
	for(auto& mesh : meshes)
		mesh.draw(shader);
}

bool Model::drawUI()
{
	bool valueChanged = false;
	std::string header = name;
	if(!isInitialized())
		header += "(loading...)";
	if(ImGui::TreeNode(header.data()))
	{
		ImGui::Text("Vertices %i", nVertices);
		ImGui::Text("Indices %i", nIndices);
		ImGui::Text("Triangles %i", nIndices / 3);
		std::string const meshHeader = std::string("Meshes(") + std::to_string(meshes.size()) + ")";
		if(ImGui::TreeNode(meshHeader.data()))
		{
			for(auto& mesh : meshes)
			{
				if(mesh.drawUI())
					valueChanged = true;
			}
			ImGui::TreePop();
		}
		std::string const materialsHeader = std::string("Materials(") + std::to_string(materials.size()) + ")";
		if(ImGui::TreeNode(materialsHeader.data()))
		{
			for(auto& material : materials)
				material->drawUI();
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	return valueChanged;
}