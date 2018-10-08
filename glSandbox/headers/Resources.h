#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Cubemap.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

#include <vector>

namespace res
{
	namespace meshes
	{
		std::vector<std::unique_ptr<Mesh>> const& getAll();
		Mesh* add(std::unique_ptr<Mesh>&&);
		void add(std::vector<std::unique_ptr<Mesh>>&&);
		Mesh* quad();
		Mesh* box();
		Mesh* boxWireframe();
	}
	namespace scenes
	{
		std::vector<std::unique_ptr<Scene>> const& getAll();
		Scene* add(std::unique_ptr<Scene>&&);
		void add(std::vector<std::unique_ptr<Scene>>&&);

	}
	namespace textures
	{
		std::vector<std::unique_ptr<Texture>> const& getAll();
		Texture* add(std::unique_ptr<Texture>&&);
		void add(std::vector<std::unique_ptr<Texture>>&&);
		Texture* placeholder();
	}
	namespace materials
	{
		std::vector<std::unique_ptr<Material>> const& getAll();
		Material* add(std::unique_ptr<Material>&&);
		void add(std::vector<std::unique_ptr<Material>>&&);
		Material* placeholder();
	}
	namespace cubemaps
	{
		//inline Cubemap skybox("skybox", {
		//	"cubemaps/skybox/right.jpg", "cubemaps/skybox/left.jpg",
		//	"cubemaps/skybox/top.jpg", "cubemaps/skybox/bottom.jpg",
		//	"cubemaps/skybox/front.jpg", "cubemaps/skybox/back.jpg"
		//	});
		//inline Cubemap mp_blizzard("mp_blizzard", {
		//	"cubemaps/mp_blizzard/blizzard_rt.tga", "cubemaps/mp_blizzard/blizzard_lf.tga",
		//	"cubemaps/mp_blizzard/blizzard_up.tga", "cubemaps/mp_blizzard/blizzard_dn.tga",
		//	"cubemaps/mp_blizzard/blizzard_ft.tga", "cubemaps/mp_blizzard/blizzard_bk.tga"
		//	});

	}
	inline std::vector<Shader>& shaders()
	{
		static std::vector<Shader> _shaders;
		return _shaders;
	}

	enum ShaderType
	{
		unlit,
		pbr,
		blinn_phong,
		phong,
		gouraud,
		flat,
		reflection,
		refraction,
		debugNormals,
		debugNormalsShowLines,
		debugTexCoords,
		debugDepthBuffer,
		skybox,
		gammaHDR,
		passthrough,
		grayscale,
		chromaticAberration,
		invert,
		convolution,
		END
	};
	void loadShaders();
	void reloadShaders();
	void drawUI(bool* open);
	Scene& importGLTF(std::string_view const filename);
}
