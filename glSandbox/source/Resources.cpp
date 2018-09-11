#include "Resources.h"
#include "ImportGLTF.h"
#include "Globals.h"

#include <glad/glad.h>
#include <imgui.h>
#include <variant>
#include <algorithm>
#include <filesystem>
#include <map>

Texture& res::textures::placeholder()
{
	static Texture placeholder("textures/placeholder.png", true);
	return placeholder;
	
}

static std::vector<std::unique_ptr<Mesh>> meshes;

std::vector<std::unique_ptr<Mesh>> const& res::meshes::getAll()
{
	return ::meshes;
}

Mesh* res::meshes::add(std::unique_ptr<Mesh>&& mesh)
{
	Mesh* ret = mesh.get();
	::meshes.push_back(std::move(mesh));
	return ret;
}

void res::meshes::add(std::vector<std::unique_ptr<Mesh>>&& meshes)
{
	::meshes.reserve(::meshes.size() + meshes.size());
	for(auto& mesh : meshes)
		::meshes.push_back(std::move(mesh));
}

struct SimpleVertex
{
	float const position[3];
};

struct Vertex
{
	float const position[3];
	float const normal[3];
	float const texcoords[2];
};

template <typename T>
Mesh::IndexBuffer buildIndexBuffer(std::vector<T>&& indices)
{
	Mesh::IndexBuffer indexBuffer;
	indexBuffer.data = reinterpret_cast<uint8_t const*>(indices.data());
	indexBuffer.count = indices.size();
	indexBuffer.size = indices.size() * sizeof(T);
	switch(sizeof(T))
	{
		case 1:
			indexBuffer.dataType = GL_UNSIGNED_BYTE;
			break;
		case 2:
			indexBuffer.dataType = GL_UNSIGNED_SHORT;
			break;
		case 4:
			indexBuffer.dataType = GL_UNSIGNED_INT;
			break;
	}
	return indexBuffer;
}

Mesh::Attributes buildAttributes(std::vector<SimpleVertex>&& vertices)
{
	Mesh::Attributes::AttributeBuffer positions;
	positions.attributeType = Mesh::AttributeType::positions;
	positions.componentSize = 3;
	positions.dataType = GL_FLOAT;
	positions.stride = sizeof(SimpleVertex);
	positions.offset = offsetof(SimpleVertex, position);

	Mesh::Attributes attributes;
	attributes.array[Mesh::AttributeType::positions] = positions;
	attributes.interleaved = true;
	attributes.data = reinterpret_cast<uint8_t const*>(vertices.data());
	attributes.size = vertices.size() * sizeof(SimpleVertex);
	return attributes;
}

Mesh::Attributes buildAttributes(std::vector<Vertex>&& vertices)
{
	Mesh::Attributes::AttributeBuffer positions;
	positions.attributeType = Mesh::AttributeType::positions;
	positions.componentSize = 3;
	positions.dataType = GL_FLOAT;
	positions.stride = sizeof(Vertex);
	positions.offset = offsetof(Vertex, position);

	Mesh::Attributes::AttributeBuffer normals;
	normals.attributeType = Mesh::AttributeType::normals;
	normals.componentSize = 3;
	normals.dataType = GL_FLOAT;
	normals.stride = sizeof(Vertex);
	normals.offset = offsetof(Vertex, normal);

	Mesh::Attributes::AttributeBuffer texcoords;
	texcoords.attributeType = Mesh::AttributeType::texcoords;
	texcoords.componentSize = 2;
	texcoords.dataType = GL_FLOAT;
	texcoords.stride = sizeof(Vertex);
	texcoords.offset = offsetof(Vertex, texcoords);

	Mesh::Attributes attributes;
	attributes.array[Mesh::AttributeType::positions] = positions;
	attributes.array[Mesh::AttributeType::normals] = normals;
	attributes.array[Mesh::AttributeType::texcoords] = texcoords;
	attributes.interleaved = true;
	attributes.data = reinterpret_cast<uint8_t const*>(vertices.data());
	attributes.size = vertices.size() * sizeof(Vertex);
	return attributes;
}

template <typename T>
Bounds calculateBounds(std::vector<T> const& vertices)
{
	Bounds bounds;
	for(auto vertex : vertices)
		bounds += vertex.position;
	return bounds;
}

Mesh* res::meshes::quad()
{
	static Mesh* quad = []() -> Mesh*{
		std::vector<Vertex> vertices = {
			{//top left
				-1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +0.0f
			},
			{//bottm right
				+1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +1.0f
			}
		};
		std::vector<uint8_t> indices = {
			0, 1, 2,
			0, 2, 3
		};
		auto bounds = calculateBounds(vertices);
		return add(std::make_unique<Mesh>(Mesh{bounds, GL_TRIANGLES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices))}));
	}();
	return quad;
}
					  
Mesh* res::meshes::box()
{
	static Mesh* box = []() -> Mesh*{
		std::vector<Vertex> vertices = {
			//front face
			{//top left
				-1.0f, +1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, -1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +1.0f
			},

			//left face
			{//top left
				-1.0f, +1.0f, -1.0f,
				-1.0f, +0.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, -1.0f,
				-1.0f, +0.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				-1.0f, -1.0f, +1.0f,
				-1.0f, +0.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				-1.0f, +1.0f, +1.0f,
				-1.0f, +0.0f, +0.0f,
				+1.0f, +1.0f
			},
			
			//back face
			{//top left
				+1.0f, +1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				+1.0f, -1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				-1.0f, -1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+1.0f, +0.0f
			},
			{//top right
				-1.0f, +1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+1.0f, +1.0f
			},
			
			//right face
			{//top left
				+1.0f, +1.0f, +1.0f,
				+1.0f, +0.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				+1.0f, -1.0f, +1.0f,
				+1.0f, +0.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, -1.0f, -1.0f,
				+1.0f, +0.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, -1.0f,
				+1.0f, +0.0f, +0.0f,
				+1.0f, +1.0f
			},
			
			//top face
			{//top left
				-1.0f, +1.0f, -1.0f,
				+0.0f, +1.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, +1.0f, +1.0f,
				+0.0f, +1.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, +1.0f, +1.0f,
				+0.0f, +1.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, -1.0f,
				+0.0f, +1.0f, +0.0f,
				+1.0f, +1.0f
			},

			//bottom face
			{//top left
				+1.0f, -1.0f, -1.0f,
				+0.0f, -1.0f, +0.0f,
				+1.0f, +1.0f
			},
			{//bottom left
				+1.0f, -1.0f, +1.0f,
				+0.0f, -1.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//bottom right
				-1.0f, -1.0f, +1.0f,
				+0.0f, -1.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//top right
				-1.0f, -1.0f, -1.0f,
				+0.0f, -1.0f, +0.0f,
				+0.0f, +1.0f
			},
		};
		std::vector<uint8_t> indices = {
			//front face
			0, 1, 2,
			0, 2, 3,
			//left face
			4, 5, 6,
			4, 6, 7,
			//back face
			8, 9, 10,
			8, 10, 11,
			//right face
			12, 13, 14,
			12, 14, 15,
			//top face
			16, 17, 18,
			16, 18, 19,
			//bottom face
			20, 21, 22,
			20, 22, 23
		};
		auto bounds = calculateBounds(vertices);
		return add(std::make_unique<Mesh>(Mesh{bounds, GL_TRIANGLES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices))}));
	}();
	return box;
}

Mesh* res::meshes::boxWireframe()
{
	static Mesh* boxWireframe = []() -> Mesh*{
		std::vector<SimpleVertex> vertices = {
			//front face
			{//top left
				-1.0f, +1.0f, +1.0f,
			},
			{//bottom left
				-1.0f, -1.0f, +1.0f,
			},
			{//bottom right
				+1.0f, -1.0f, +1.0f,
			},
			{//top right
				+1.0f, +1.0f, +1.0f,
			},

			//back face
			{//top left
				+1.0f, +1.0f, -1.0f,
			},
			{//bottom left
				+1.0f, -1.0f, -1.0f,
			},
			{//bottom right
				-1.0f, -1.0f, -1.0f,
			},
			{//top right
				-1.0f, +1.0f, -1.0f,
			}
		};
		std::vector<uint8_t> indices = {
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			0, 7,
			7, 6,
			6, 1, 
			2, 5,
			5, 4,
			4, 3,
			7, 4,
			5, 6
		};
		auto bounds = calculateBounds(vertices);
		return add(std::make_unique<Mesh>(Mesh{bounds, GL_LINES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices))}));
	}();
	return boxWireframe;
}
 
Mesh* res::meshes::grid(int resolution)
{
	static std::unordered_map<int, std::unique_ptr<Mesh>> gridCache;
	if(gridCache.find(resolution) == gridCache.end())
	{ 
		std::vector<SimpleVertex> vertices;
		std::vector<uint16_t> indices;
		int const sideLength = resolution + 1;
		float const step = 2.0f / resolution;
		int idx = 0;
		for(int x = 0; x < sideLength; x++, idx+=2)
		{
			vertices.push_back({-1.0f + x * step, 0.0f, -1.0f});
			vertices.push_back({-1.0f + x * step, 0.0f, +1.0f});
			indices.push_back(idx);
			indices.push_back(idx + 1);
		}
		indices.push_back(0);
		indices.push_back((sideLength - 1) * 2);
		indices.push_back(1);
		indices.push_back(sideLength * 2 - 1);
		for(int z = 1; z < sideLength - 1; z++, idx += 2)
		{
			vertices.push_back({-1.0f, 0.0f, -1.0f + z * step});
			vertices.push_back({+1.0f, 0.0f, -1.0f + z * step});
			indices.push_back(idx);
			indices.push_back(idx + 1);
		}
		auto bounds = calculateBounds(vertices);
		auto grid = std::make_unique<Mesh>(Mesh{bounds, GL_LINES, buildAttributes(std::move(vertices)), buildIndexBuffer(std::move(indices))});
		gridCache.insert({resolution, std::move(grid)});
	}
	return gridCache[resolution].get();
}

static std::vector<std::unique_ptr<Scene>> scenes;

std::vector<std::unique_ptr<Scene>> const& res::scenes::getAll()
{
	return ::scenes;
}

void res::scenes::add(std::unique_ptr<Scene>&& scene)
{
	::scenes.push_back(std::move(scene));
}

void removeScene(Scene* scene)
{
	scenes.erase(std::remove_if(scenes.begin(), scenes.end(), [&](std::unique_ptr<Scene>& val){ return val.get() == scene; }), scenes.end());
}

void res::scenes::add(std::vector<std::unique_ptr<Scene>>&& scenes)
{
	::scenes.reserve(::scenes.size() + scenes.size());
	for(auto& scene : scenes)
		::scenes.push_back(std::move(scene));
}

void res::loadShaders()
{
	for(int i = 0; i < ShaderType::END; i++)
	{
		switch(i)
		{
			case ShaderType::unlit:
				shaders().emplace_back("shaders/unlit.vert", "shaders/unlit.frag");
				break;
			case ShaderType::blinn_phong:
				shaders().emplace_back("shaders/phong.vert", "shaders/blinn-phong.frag");
				break;
			case ShaderType::phong:
				shaders().emplace_back("shaders/phong.vert", "shaders/phong.frag");
				break;
			case ShaderType::gouraud:
				shaders().emplace_back("shaders/gouraud.vert", "shaders/gouraud.frag");
				break;
			case ShaderType::flat:
				shaders().emplace_back("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
				break;
			case ShaderType::reflection:
				shaders().emplace_back("shaders/reflection.vert", "shaders/reflection.frag");
				break;
			case ShaderType::refraction:
				shaders().emplace_back("shaders/refraction.vert", "shaders/refraction.frag");
				break;
			case ShaderType::debugNormals:
				shaders().emplace_back("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
				break;
			case ShaderType::debugNormalsShowLines:
				shaders().emplace_back("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
				break;
			case ShaderType::debugTexCoords:
				shaders().emplace_back("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
				break;
			case ShaderType::debugDepthBuffer:
				shaders().emplace_back("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
				break;
			case ShaderType::skybox:
				shaders().emplace_back("shaders/skybox.vert", "shaders/skybox.frag");
				break;
			case ShaderType::gammaHDR:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppGammaHDR.frag");
				break;
			case ShaderType::passthrough:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppPassthrough.frag");
				break;
			case ShaderType::grayscale:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppBW.frag");
				break;
			case ShaderType::chromaticAberration:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
				break;
			case ShaderType::invert:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppInvert.frag");
				break;
			case ShaderType::convolution:
				shaders().emplace_back("shaders/pp.vert", "shaders/ppConvolution.frag");
				break;
			default:
				assert(false);//means we forgot to add a shader
		}
	}
}

void res::reloadShaders()
{
	for(int i = 0; i < ShaderType::END; i++)
		shaders()[i].reload();
}

void drawImportWindow(bool *open)
{
	if(!*open)
		return;
	ImGui::Begin("Import", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	static std::filesystem::path path{std::filesystem::current_path()};
	ImGui::Text(path.generic_string().data());
	if(ImGui::Selectable(".."))
		path = path.parent_path();
	std::vector<std::filesystem::directory_entry> folders;
	std::vector<std::filesystem::directory_entry> files;
	for(auto const& part : std::filesystem::directory_iterator(path))
	{
		if(part.is_directory())
			folders.push_back(part);
		else
			files.push_back(part);
	}
	ImVec2 elementSize{ImGui::GetContentRegionAvail().x, 0.0f};
	for(auto const& folder : folders)
	{
		if(ImGui::Selectable(folder.path().filename().generic_string().data()))
			path = folder.path();
	}
	ImGui::Separator();
	for(auto const& file : files)
	{
		std::string filename = file.path().filename().generic_string();
		if(file.path().extension() == ".gltf")
		{
			if(ImGui::Selectable(filename.data()))
				settings::mainRenderer().setCamera(res::importGLTF(file.path().generic_string()).getAll<Camera>()[0]);
		}
		else
		{
			ImGui::Text(filename.data());
		}
	}
	ImGui::End();
}

void res::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Resources", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	float const scrollAreaHeight = ImGui::GetTextLineHeightWithSpacing() * 8;
	float const scrollAreaWidth = ImGui::GetTextLineHeightWithSpacing() * 15;
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(-1, scrollAreaWidth);
	static bool importWindowOpen = false;
	if(ImGui::Button("Import"))
		importWindowOpen = true;
	drawImportWindow(&importWindowOpen);
	ImGui::SameLine();
	if(ImGui::Button("Reload Shaders"))
		reloadShaders();
	static std::variant<Scene*, Mesh*> selected;
	ImGui::BeginChild("###Resources");

	ImGui::Text("Scenes");
	ImGui::SameLine();
	if(ImGui::SmallButton("New"))
		scenes::add(std::make_unique<Scene>());
	ImGui::BeginChild("###Scenes", {0, scrollAreaHeight}, true);
	int id = 0;
	for(auto& scene : scenes::getAll())
	{
		ImGui::PushID(id++);
		bool active = false;
		if(std::holds_alternative<Scene*>(selected))
			active = std::get<Scene*>(selected) == scene.get();
		if(ImGui::Selectable(scene->name.get().data(), active))
			selected = scene.get();
		if(ImGui::BeginPopupContextItem())
		{
			bool showRemoveModal = ImGui::Selectable("Remove");
			ImGui::EndPopup();
			if(showRemoveModal) ImGui::OpenPopup("Are you sure?");
		}
		if(ImGui::BeginPopupModal("Are you sure?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize({100, ImGui::GetTextLineHeight() * 5});
			ImVec2 buttonSize{ImGui::GetContentRegionAvailWidth() * 0.5f, ImGui::GetContentRegionAvail().y};
			if(ImGui::Button("Yes", buttonSize))
			{
				if(active)
					selected = static_cast<Scene*>(nullptr);
				removeScene(scene.get());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if(ImGui::Button("No", buttonSize))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	ImGui::EndChild();
	ImGui::NewLine();

	ImGui::Text("Meshes");
	ImGui::BeginChild("###Meshes", {0, scrollAreaHeight}, true);
	for(auto& mesh : meshes::getAll())
	{
		ImGui::PushID(id++);
		bool active = false;
		if(std::holds_alternative<Mesh*>(selected))
			active = std::get<Mesh*>(selected) == mesh.get();
		if(ImGui::Selectable(mesh->name.get().data(), active))
			selected = mesh.get();
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::EndChild();

	ImGui::NextColumn();
	std::visit([](auto selected){
		ImGui::Text(selected ? selected->name.get().data() : "No selection...");
		ImGui::BeginChild("###Edit Resource", {0, 0}, true);
		if(selected) selected->drawUI();
		ImGui::EndChild();
	}, selected);

	ImGui::Columns(1);
	ImGui::End();
}

Scene& res::importGLTF(std::string_view const filename)
{
	auto asset = import(filename);
	auto ret = asset.scenes[0].get();//active scene
	scenes::add(std::move(asset.scenes));
	meshes::add(std::move(asset.meshes));
	return *ret;
}

