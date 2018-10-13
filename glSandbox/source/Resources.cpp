#include "Resources.h"
#include "ImportGLTF.h"
#include "Globals.h"
#include "MaterialPBRMetallicRoughness.h"
#include "Prop.h"
#include "FileSelector.h"

#include <glad/glad.h>
#include <imgui.h>
#include <variant>
#include <algorithm>
#include <filesystem>
#include <map>

Mesh* ResourceManager<Mesh>::quad()
{
	static Mesh* ret = []() -> Mesh* {
		std::vector<Vertex> vertices = {
			{//top left
				glm::vec3{-1.0f, +1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottm right
				glm::vec3{+1.0f, -1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +1.0f}
			}
		};
		std::vector<uint8_t> indices = {
			0, 1, 2,
			0, 2, 3
		};
		auto mesh = std::make_unique<Mesh>(
			calculateBounds(vertices), GL_TRIANGLES,
			buildAttributes(std::move(vertices)), 
			buildIndexBuffer(std::move(indices)));
		mesh->name.set("quad");
		return add(std::move(mesh));
	}();
	return ret;
}

Mesh* ResourceManager<Mesh>::box()
	{
		static Mesh* ret = []() -> Mesh* {
			std::vector<Vertex> vertices = {
				//front face
				{//top left
					glm::vec3{-1.0f, +1.0f, +1.0f},
					glm::vec3{+0.0f, +0.0f, +1.0f},
					glm::vec2{+0.0f, +1.0f}
				},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//left face
			{//top left
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, +1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//back face
			{//top left
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//right face
			{//top left
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//top face
			{//top left
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//bottom face
			{//top left
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
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
			auto mesh = std::make_unique<Mesh>(
				calculateBounds(vertices), GL_TRIANGLES,
				buildAttributes(std::move(vertices)),
				buildIndexBuffer(std::move(indices)));
			mesh->name.set("box");
			return add(std::move(mesh));
		}();
		return ret;
	}

Mesh* ResourceManager<Mesh>::boxWireframe()
	{
		static Mesh* ret = []() -> Mesh* {
			std::vector<SimpleVertex> vertices = {
				//front face
				{//top left
					glm::vec3{-1.0f, +1.0f, +1.0f}
				},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +1.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, +1.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +1.0f}
			},

			//back face
			{//top left
				glm::vec3{+1.0f, +1.0f, -1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, -1.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, -1.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, -1.0f}
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
			auto mesh = std::make_unique<Mesh>(
				calculateBounds(vertices), GL_LINES,
				buildAttributes(std::move(vertices)),
				buildIndexBuffer(std::move(indices)));
			mesh->name.set("box_wireframe");
			return (add(std::move(mesh)));
		}();
		return ret;
	}

Texture* ResourceManager<Texture>::debug()
{
	static auto ret = add(std::make_unique<Texture>("textures/debug.png", false));
	return ret;

}

Material* ResourceManager<Material>::basic()
{
	static auto ret = [&]() -> Material* {
		auto material = std::make_unique<MaterialPBRMetallicRoughness>();
		material->setBaseColor(ResourceManager<Texture>::debug(), std::nullopt);
		material->name.set("basic");
		return add(std::move(material));
	}();
	return ret;
}

Cubemap* ResourceManager<Cubemap>::skybox()
{
	static auto ret= [&]() -> Cubemap* {
		std::array<Texture, 6> faces = {
			Texture{"cubemaps/skybox/right.jpg", false},
			Texture{"cubemaps/skybox/left.jpg", false},
			Texture{"cubemaps/skybox/top.jpg", false},
			Texture{"cubemaps/skybox/bottom.jpg", false},
			Texture{"cubemaps/skybox/front.jpg", false},
			Texture{"cubemaps/skybox/back.jpg", false}
		};
		auto cubemap = std::make_unique<Cubemap>(std::move(faces));
		cubemap->name.set("skybox");
		return add(std::move(cubemap));
	}();
	return ret;

}

Scene* ResourceManager<Scene>::importGLTF(std::string_view const filename)
{
	auto asset = import(filename);
	auto ret = asset.scenes[0].get();//active scene
	add(std::move(asset.scenes));
	ResourceManager<Mesh>::add(std::move(asset.meshes));
	ResourceManager<Texture>::add(std::move(asset.textures));
	ResourceManager<Material>::add(std::move(asset.materials));
	return ret;
}

Scene* ResourceManager<Scene>::test()
{
	static auto ret = [&]() -> Scene*{
		auto scene = std::make_unique<Scene>();
		scene->name.set("test");
		auto prop = std::make_unique<Prop>(ResourceManager<Mesh>::box());
		prop->setLocalRotation({45.0f, 45.0f, 0.0f});
		prop->setLocalTranslation({0.0f, 1.0f, 0.0f});
		scene->getRoot()->addChild(std::move(prop));
		auto floor = std::make_unique<Prop>(ResourceManager<Mesh>::box());
		floor->setLocalScale({30.0f, 0.5f, 30.0f});
		floor->setLocalTranslation({0.0f, -2.0f, 0.0f});
		scene->getRoot()->addChild(std::move(floor));
		return add(std::move(scene));
	}();
	return ret;
}

void ResourceManager<Shader>::reloadAll()
{
	for(auto& shader : getAll())
		shader->reload();
}

bool ResourceManager<Shader>::isLightingShader(Shader* shader)
{
	return 
		shader == pbr() || 
		shader == blinnPhong() ||
		shader == phong() ||
		shader == gouraud() ||
		shader == flat();
}

Shader* ResourceManager<Shader>::unlit()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/unlit.vert", "shaders/unlit.frag");
		shader->name.set("Unlit");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::pbr()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbr.frag");
		shader->name.set("PBR");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::blinnPhong()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/phong.vert", "shaders/blinn-phong.frag");
		shader->name.set("Blinn-Phong");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::phong()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/phong.vert", "shaders/phong.frag");
		shader->name.set("Phong");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::gouraud()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/gouraud.vert", "shaders/gouraud.frag");
		shader->name.set("Gouraud");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::flat()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
		shader->name.set("Flat");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::reflection()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/reflection.vert", "shaders/reflection.frag");
		shader->name.set("Reflection");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::refraction()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/refraction.vert", "shaders/refraction.frag");
		shader->name.set("Refraction");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::debugNormals()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
		shader->name.set("Debug Normals");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::debugNormalsShowLines()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
		shader->name.set("Debug Normals Show Lines");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::debugTexCoords()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
		shader->name.set("Debug Texture Coordinates");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::debugDepthBuffer()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
		shader->name.set("Debug Depth Buffer");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::shadowMapping()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/shadowMapping.vert", "shaders/shadowMapping.frag");
		shader->name.set("Debug Depth Buffer");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::skybox()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
		shader->name.set("Skybox");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::gammaHDR()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppGammaHDR.frag");
		shader->name.set("Gamma / HDR");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::passthrough()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppPassthrough.frag");
		shader->name.set("Passthrough");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::grayscale()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppBW.frag");
		shader->name.set("Grayscale");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::chromaticAberration()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
		shader->name.set("Chromatic Aberration");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::invert()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppInvert.frag");
		shader->name.set("Invert");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ResourceManager<Shader>::convolution()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppConvolution.frag");
		shader->name.set("Convolution");
		return add(std::move(shader));
	}();
	return ret;
}

void initializeResources()
{
	ResourceManager<Mesh>::quad();
	ResourceManager<Mesh>::box();
	ResourceManager<Mesh>::boxWireframe();

	ResourceManager<Texture>::debug();

	ResourceManager<Material>::basic();

	ResourceManager<Cubemap>::skybox();

	ResourceManager<Scene>::test();

	ResourceManager<Shader>::unlit();
	ResourceManager<Shader>::pbr();
	ResourceManager<Shader>::blinnPhong();
	ResourceManager<Shader>::phong();
	ResourceManager<Shader>::gouraud();
	ResourceManager<Shader>::flat();
	ResourceManager<Shader>::reflection();
	ResourceManager<Shader>::refraction();
	ResourceManager<Shader>::debugNormals();
	ResourceManager<Shader>::debugNormalsShowLines();
	ResourceManager<Shader>::debugTexCoords();
	ResourceManager<Shader>::debugDepthBuffer();
	ResourceManager<Shader>::skybox();
	ResourceManager<Shader>::gammaHDR();
	ResourceManager<Shader>::passthrough();
	ResourceManager<Shader>::grayscale();
	ResourceManager<Shader>::chromaticAberration();
	ResourceManager<Shader>::invert();
	ResourceManager<Shader>::convolution();
}

class CubemapLoaderFunctor
{
private:
	inline static std::array<std::string, 6> const faceNames = {
		"Face +X", "Face -X",
		"Face +Y", "Face -Y",
		"Face +Z", "Face -Z"
	};
	std::array<std::optional<Texture>, 6> faces;
	bool allLoaded = true;
	FileSelector* browser = nullptr;
	int activeFace = -1;
	bool explorerOpen = true;

public:
	CubemapLoaderFunctor() = default;
	~CubemapLoaderFunctor()
	{
		if(browser)
			delete browser;
	}

public:
	bool isDone() const
	{
		return !explorerOpen;
	}

	void operator()()
	{
		ImGui::Begin("Import Cubemap", &explorerOpen, ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
		allLoaded = true;
		for(int i = 0; i < 6; ++i)
		{
			if(ImGui::Button(faceNames[i].data()))
			{
				if(activeFace == -1)
				{
					auto oldPath = std::filesystem::current_path();
					if(browser)
					{
						oldPath = browser->getFile();
						delete browser;
					}
					browser = new FileSelector({".jpg", ".png", ".tga"}, oldPath);
					activeFace = i;
				}
			}
			ImGui::SameLine();
			if(faces[i])
			{
				ImGui::Text("( %s )", faces[i]->name.get().data());
			}
			else
			{
				allLoaded = false;
				ImGui::Text("(None)");
			}
		}

		if(activeFace != -1)
		{
			browser->drawUI();
			if(browser->fileChosen())
			{
				faces[activeFace] = Texture(browser->getFile().string(), false);
				activeFace = -1;
			}
			else if(browser->cancelled())
			{
				activeFace = -1;
			}
		}

		if(allLoaded && ImGui::Button("Ok"))
		{
			ResourceManager<Cubemap>::add(std::make_unique<Cubemap>(
				std::array<Texture, 6>{
				std::move(*faces[0]),
					std::move(*faces[1]),
					std::move(*faces[2]),
					std::move(*faces[3]),
					std::move(*faces[4]),
					std::move(*faces[5])
			}));
			explorerOpen = false;
		}
		ImGui::End();
	}

};
void drawResourcesUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Resources", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	float const scrollAreaWidth = ImGui::GetTextLineHeightWithSpacing() * 15;
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(-1, scrollAreaWidth);
	static void(*importCallback)(std::filesystem::path) = nullptr;
	static FileSelector* browser = nullptr;

	if(browser && importCallback)
	{
		browser->drawUI();
		if(browser->fileChosen())
		{
			importCallback(browser->getFile());
			delete browser;
			browser = nullptr;
			importCallback = nullptr;
		}
		else if(browser->cancelled())
		{
			delete browser;
			browser = nullptr;
			importCallback = nullptr;
		}
	}
	static CubemapLoaderFunctor* cubemapBrowser = nullptr;
	if(cubemapBrowser)
	{
		(*cubemapBrowser)();
		if(cubemapBrowser->isDone())
		{
			delete cubemapBrowser;
			cubemapBrowser = nullptr;
		}
	}
	static std::variant<Scene*, Mesh*, Texture*, Material*, Cubemap*, Shader*> selected;
	ImGui::BeginChild("###Resources");
	int id = 0;
	auto drawResourcesWithCallback = [&id](std::string const& title, auto const& container, auto extraUI){
		IDGuard idGuard(title.data());
		static float const scrollAreaHeight = ImGui::GetTextLineHeightWithSpacing() * 8;
		ImGui::AlignTextToFramePadding();
		ImGui::Text(title.data());
		extraUI();
		ImGui::BeginChild(("###" + title).data(), {0, scrollAreaHeight}, true);
		for(auto& resource : container)
		{
			ImGui::PushID(id++);
			bool active = false;
			using T = decltype(resource.get());
			if(std::holds_alternative<T>(selected))
				active = std::get<T>(selected) == resource.get();
			if(ImGui::Selectable(resource->name.get().data(), active))
				selected = resource.get();
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::NewLine();
	};
	auto drawResources = [&drawResourcesWithCallback](std::string const& title, auto const& container){
		drawResourcesWithCallback(title, container, [](){});
	};

	drawResourcesWithCallback("Scenes", ResourceManager<Scene>::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			if(!browser && !importCallback)
			{
				browser = new FileSelector({".gltf"});
				importCallback = [](std::filesystem::path file)	{
					settings::mainRenderer().setCamera(
						ResourceManager<Scene>::importGLTF(file.string())->getAll<Camera>().front());
				};
			}
		}
		ImGui::SameLine();
		if(ImGui::SmallButton("New"))
			ResourceManager<Scene>::add(std::make_unique<Scene>());
	});
	drawResources("Meshes", ResourceManager<Mesh>::getAll());
	drawResourcesWithCallback("Textures", ResourceManager<Texture>::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			if(!browser && !importCallback)
			{
				browser = new FileSelector({".jpg", ".png", ".tga"});
				importCallback = [](std::filesystem::path file){
					ResourceManager<Texture>::add(std::make_unique<Texture>(file.string(), true));
				};
			}
		}
	});
	drawResources("Materials", ResourceManager<Material>::getAll());
	drawResourcesWithCallback("Cubemaps", ResourceManager<Cubemap>::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			cubemapBrowser = new CubemapLoaderFunctor();
		}
	});
	drawResourcesWithCallback("Shaders", ResourceManager<Shader>::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Reload"))
			ResourceManager<Shader>::reloadAll();
	});

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


