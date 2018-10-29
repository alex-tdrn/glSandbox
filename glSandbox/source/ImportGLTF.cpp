#include "ImportGLTF.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Prop.h"
#include "MaterialPBRMetallicRoughness.h"

#include <fx/gltf.h>
#include <numeric>
#include <glm/gtc/quaternion.hpp>
#include <filesystem>

using namespace fx;
using PrimitivesMap = std::map<gltf::Primitive const*, Mesh*>;

uint32_t calculateElementSize(gltf::Accessor const& accessor);
uint32_t componentSize(gltf::Accessor::Type type);
GLenum gltfToGLType(gltf::Accessor::ComponentType type);
std::pair<std::vector<std::unique_ptr<Mesh>>, PrimitivesMap> loadMeshes(gltf::Document const& doc);
std::pair<std::vector<std::unique_ptr<Texture>>, std::vector<std::unique_ptr<Material>>>
	loadTexturesAndMaterials(gltf::Document const& doc, std::filesystem::path const& currentPath);
std::vector<std::unique_ptr<Scene>> loadScenes(gltf::Document const& doc, 
	PrimitivesMap const& primitivesMap, 
	std::vector<std::unique_ptr<Mesh>>& loadedMeshes,
	std::vector<std::unique_ptr<Material>> const& materials);

Asset import(std::string_view const& filename)
{
	std::filesystem::path currentPath = filename;
	gltf::ReadQuotas readQuota;
	readQuota.MaxBufferByteLength = std::numeric_limits<uint32_t>::max();
	gltf::Document doc = gltf::LoadFromText(filename.data(), readQuota);
	
	auto [meshes, primitivesMap] = loadMeshes(doc);
	auto [textures, materials] = loadTexturesAndMaterials(doc, currentPath.parent_path().string());

	auto scenes = loadScenes(doc, primitivesMap, meshes, materials);
	std::filesystem::path file(filename);
	std::string name = file.stem().string();
	for(int i = 0; i < scenes.size(); i++)
	{
		if(i > 0)
			scenes[i]->setName(name + "#" + std::to_string(i));
		else
			scenes[i]->setName(name);
	}
	
	return {std::move(scenes), std::move(meshes), std::move(textures), std::move(materials)};
}

std::unique_ptr<Node> loadNode(gltf::Document const& doc, 
	size_t const idx, PrimitivesMap const& primitivesMap,
	std::vector<std::unique_ptr<Material>> const& materials)
{
	auto const& node = doc.nodes[idx];
	std::unique_ptr<Node> n;

	if(node.mesh != -1)
	{
		auto const& mesh = doc.meshes[node.mesh];
		if(mesh.primitives.size() == 1)
		{
			int materialIndex = mesh.primitives[0].material;
			if(materialIndex != -1)
				n = std::make_unique<Prop>(primitivesMap.at(&mesh.primitives[0]), materials[materialIndex].get());
			else
				n = std::make_unique<Prop>(primitivesMap.at(&mesh.primitives[0]));
		}
		else
		{
			n = std::make_unique<TransformedNode>();
			for(auto const& primitive :doc.meshes[node.mesh].primitives)
			{
				if(primitive.material != -1)
					n->addChild(std::make_unique<Prop>(primitivesMap.at(&mesh.primitives[0]), materials[primitive.material].get()));
				else
					n->addChild(std::make_unique<Prop>(primitivesMap.at(&primitive)));
			}
		}
	}
	else
	{
		n = std::make_unique<TransformedNode>();
	}
	if(!node.name.empty())
		n->setName(node.name);
	glm::mat4 transformation{1.0f};
	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			transformation[col][row] = node.matrix[row + col * 4];
		}
	}
	glm::vec3 t{node.translation[0], node.translation[1], node.translation[2]};
	glm::quat r{node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]};
	glm::vec3 s{node.scale[0], node.scale[1], node.scale[2]};

	transformation = glm::scale(transformation, s);
	transformation *= glm::mat4_cast(r);
	transformation = glm::translate(transformation, t);
	assert(transformation[0][3] == 0);
	assert(transformation[1][3] == 0);
	assert(transformation[2][3] == 0);
	assert(transformation[3][3] == 1);
	n->setLocalTransformation(std::move(transformation));
	for(auto childIdx : node.children)
		n->addChild(loadNode(doc, childIdx, primitivesMap, materials));

	return n;
}

std::vector<std::unique_ptr<Scene>> loadScenes(gltf::Document const& doc, 
	PrimitivesMap const& primitivesMap, 
	std::vector<std::unique_ptr<Mesh>>& loadedMeshes,
	std::vector<std::unique_ptr<Material>> const& materials)
{
	std::vector<std::unique_ptr<Scene>> scenes;
	for(auto const& scene : doc.scenes)
	{
		std::vector<std::unique_ptr<Node>> nodes;
		for(auto const nodeIdx : scene.nodes)
			nodes.emplace_back(loadNode(doc, nodeIdx, primitivesMap, materials));
		auto s = std::make_unique<Scene>(std::make_unique<TransformedNode>(std::move(nodes)));
		if(!scene.name.empty())
			s->setName(scene.name);
		scenes.emplace_back(std::move(s));
	}
	return scenes;
}

std::pair<std::vector<std::unique_ptr<Mesh>>, PrimitivesMap> loadMeshes(gltf::Document const& doc)
{
	std::vector<std::unique_ptr<Mesh>> meshes;
	PrimitivesMap primitivesMap;
	for(auto const& mesh : doc.meshes)
	{
		int idx = 1;
		for(auto const& primitive : mesh.primitives)
		{
			Mesh::Attributes attributes;
			glm::vec3 min;
			glm::vec3 max;
			for(auto const& attribute : primitive.attributes)
			{
				Mesh::Attributes::AttributeBuffer attributeBuffer;
				if(attribute.first == "POSITION")
				{
					attributeBuffer.attributeType = Mesh::AttributeType::positions;
					auto const& aMin = doc.accessors[attribute.second].min;
					min = glm::vec3(aMin[0], aMin[1], aMin[2]);
					auto const& aMax = doc.accessors[attribute.second].max;
					max = glm::vec3(aMax[0], aMax[1], aMax[2]);
				}
				else if(attribute.first == "NORMAL")
					attributeBuffer.attributeType = Mesh::AttributeType::normals;
				else if(attribute.first == "TANGENT")
					attributeBuffer.attributeType = Mesh::AttributeType::tangents;
				else if(attribute.first == "TEXCOORD_0")
					attributeBuffer.attributeType = Mesh::AttributeType::texcoords;
				else
					continue;
				auto const& accessor = doc.accessors[attribute.second];
				auto const& bufferView = doc.bufferViews[accessor.bufferView];
				auto const& buffer = doc.buffers[bufferView.buffer];
				auto const elementSize = calculateElementSize(accessor);
				attributeBuffer.data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
				attributeBuffer.size = accessor.count * elementSize;
				if(bufferView.byteStride > elementSize)//interleaved data
				{
					attributeBuffer.stride = bufferView.byteStride;
					attributeBuffer.offset = accessor.byteOffset;
					attributes.interleaved = true;
					attributes.data = &buffer.data[bufferView.byteOffset];
					attributes.size = bufferView.byteLength;
				}
				else
					attributeBuffer.stride = elementSize;
				attributeBuffer.componentSize = componentSize(accessor.type);
				attributeBuffer.dataType = gltfToGLType(accessor.componentType);
				attributes.array[attributeBuffer.attributeType] = std::move(attributeBuffer);
			}
			std::optional<Mesh::IndexBuffer> indices = std::nullopt;
			if(primitive.indices != -1)
			{
				indices = Mesh::IndexBuffer{};
				auto const& accessor = doc.accessors[primitive.indices];
				auto const& bufferView = doc.bufferViews[accessor.bufferView];
				auto const& buffer = doc.buffers[bufferView.buffer];
				indices->data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
				indices->count = accessor.count;
				indices->size = accessor.count * calculateElementSize(accessor);
				indices->dataType = gltfToGLType(accessor.componentType);
			}

			GLenum drawMode = [&](){
				switch(primitive.mode)
				{
					case gltf::Primitive::Mode::Points:
						return GL_POINTS;
					case gltf::Primitive::Mode::Lines:
						return GL_LINES;
					case gltf::Primitive::Mode::LineLoop:
						return GL_LINE_LOOP;
					case gltf::Primitive::Mode::TriangleFan:
						return GL_TRIANGLE_FAN;
					case gltf::Primitive::Mode::TriangleStrip:
						return GL_TRIANGLE_STRIP;
					default:
						return GL_TRIANGLES;
				}
			}();
			auto m = std::make_unique<Mesh>(Bounds{min, max}, drawMode, std::move(attributes), std::move(indices));
			if(!mesh.name.empty())
				m->setName(mesh.name + "#" + std::to_string(idx++));
			primitivesMap[&primitive] = m.get();
			meshes.push_back(std::move(m));
		}
	}
	return {std::move(meshes), std::move(primitivesMap)};
}



template <size_t N>
auto getFactor(std::array<float, N> const& factor)
{
	if constexpr(N == 0)
		return std::nullopt;
	else if constexpr(N == 3)
		return glm::vec3(factor[0], factor[1], factor[2]);
	else if constexpr(N == 4)
		return glm::vec4(factor[0], factor[1], factor[2], factor[3]);

};

std::pair<std::vector<std::unique_ptr<Texture>>, std::vector<std::unique_ptr<Material>>>
	loadTexturesAndMaterials(gltf::Document const& doc, std::filesystem::path const& currentPath)
{
	std::vector<std::unique_ptr<Texture>> textures;
	std::vector<std::unique_ptr<Material>> materials;
	auto getMap = [&textures, &doc, &currentPath](gltf::Material::Texture const& texture, bool linear = true) -> Texture*{
		if(texture.empty())
		{
			return nullptr;
		}
		else
		{
			auto image = doc.images[texture.index];
			if(image.IsEmbeddedResource())//TODO
				assert(false);
			auto path = currentPath / image.uri;
			auto texture = std::make_unique<Texture>(path.string(), linear);
			texture->setName(image.name.empty() ? path.filename().string() : image.name);
			textures.push_back(std::move(texture));
			return textures.back().get();
		}
	};

	for(auto& material : doc.materials)
	{
		std::unique_ptr<Material> _material;
		if(auto materialMR = material.pbrMetallicRoughness; !materialMR.empty())
		{
			auto _materialMR = new MaterialPBRMetallicRoughness;
			_materialMR->setBaseColorMap(getMap(materialMR.baseColorTexture, false));
			_materialMR->setBaseColorFactor(getFactor(materialMR.baseColorFactor));
			_materialMR->setMetallicRoughnessMap(getMap(materialMR.metallicRoughnessTexture));
			_materialMR->setMetallicFactor(materialMR.metallicFactor);
			_materialMR->setRoughnessFactor(materialMR.roughnessFactor);
			_material.reset(_materialMR);
		}
		else
		{
			_material = std::make_unique<Material>();
			//assert(false);//TODO
		}

		if(!material.name.empty())
			_material->setName(material.name);
		_material->setNormalMap(getMap(material.normalTexture));
		_material->setOcclusionMap(getMap(material.occlusionTexture));
		_material->setEmissiveMap(getMap(material.emissiveTexture, false));
		_material->setEmissiveFactor(getFactor(material.emissiveFactor));
		if(material.doubleSided)
		{
			//TODO
		}
		materials.push_back(std::move(_material));
	}

	return {std::move(textures), std::move(materials)};
}

GLenum gltfToGLType(gltf::Accessor::ComponentType type)
{
	switch(type)
	{
		case gltf::Accessor::ComponentType::Byte:
			return GL_BYTE;
		case gltf::Accessor::ComponentType::UnsignedByte:
			return GL_UNSIGNED_BYTE;
		case gltf::Accessor::ComponentType::Short:
			return GL_SHORT;
		case gltf::Accessor::ComponentType::UnsignedShort:
			return GL_UNSIGNED_SHORT;
		case gltf::Accessor::ComponentType::Float:
			return GL_FLOAT;
		case gltf::Accessor::ComponentType::UnsignedInt:
			return GL_UNSIGNED_INT;
	}
	assert(0);
	return 0;
}

uint32_t componentSize(gltf::Accessor::Type type)
{
	switch(type)
	{
		case gltf::Accessor::Type::Scalar:
			return 1;
		case gltf::Accessor::Type::Vec2:
			return 2;
		case gltf::Accessor::Type::Vec3:
			return 3;
		case gltf::Accessor::Type::Vec4:
		case gltf::Accessor::Type::Mat2:
			return 4;
		case gltf::Accessor::Type::Mat3:
			return 9;
		case gltf::Accessor::Type::Mat4:
			return 16;
	}
	assert(0);
	return 0;
}
uint32_t calculateElementSize(gltf::Accessor const& accessor)
{
	uint32_t elementSize = 0;
	switch(accessor.componentType)
	{
		case gltf::Accessor::ComponentType::Byte:
		case gltf::Accessor::ComponentType::UnsignedByte:
			elementSize = 1;
			break;
		case gltf::Accessor::ComponentType::Short:
		case gltf::Accessor::ComponentType::UnsignedShort:
			elementSize = 2;
			break;
		case gltf::Accessor::ComponentType::Float:
		case gltf::Accessor::ComponentType::UnsignedInt:
			elementSize = 4;
			break;
	}
	return elementSize * componentSize(accessor.type);
}