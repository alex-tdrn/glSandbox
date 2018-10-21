#include "ResourceManager.h"
#include "TextureManager.h"
#include "CubemapManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "ShaderManager.h"
#include "SceneManager.h"
#include "Globals.h"
#include "FileSelector.h"

#include <glad/glad.h>
#include <imgui.h>
#include <variant>
#include <algorithm>
#include <filesystem>
#include <string>
#include <array>

void initializeResources()
{
	TextureManager::initialize();
	CubemapManager::initialize();
	MaterialManager::initialize();
	MeshManager::initialize();
	ShaderManager::initialize();
	SceneManager::initialize();
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
			CubemapManager::add(std::make_unique<Cubemap>(
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

	drawResourcesWithCallback("Scenes", SceneManager::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			if(!browser && !importCallback)
			{
				browser = new FileSelector({".gltf"});
				importCallback = [](std::filesystem::path file)	{
					settings::mainRenderer().setCamera(
						SceneManager::importGLTF(file.string())->getAll<Camera>().front());
				};
			}
		}
		ImGui::SameLine();
		if(ImGui::SmallButton("New"))
			SceneManager::add(std::make_unique<Scene>());
	});
	drawResources("Meshes", MeshManager::getAll());
	drawResourcesWithCallback("Textures", TextureManager::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			if(!browser && !importCallback)
			{
				browser = new FileSelector({".jpg", ".png", ".tga", ".hdr"});
				importCallback = [](std::filesystem::path file){
					TextureManager::add(std::make_unique<Texture>(file.string(), true));
				};
			}
		}
	});
	drawResources("Materials", MaterialManager::getAll());
	drawResourcesWithCallback("Cubemaps", CubemapManager::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Import"))
		{
			cubemapBrowser = new CubemapLoaderFunctor();
		}
	});
	drawResourcesWithCallback("Shaders", ShaderManager::getAll(), [](){
		ImGui::SameLine();
		if(ImGui::SmallButton("Reload"))
			ShaderManager::reloadAll();
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


