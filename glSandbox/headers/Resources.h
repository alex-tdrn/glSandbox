#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Cubemap.h"
#include "Mesh.h"
#include "Texture.h"

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
		Mesh* grid(int resolution);
	}
	namespace scenes
	{
		std::vector<std::unique_ptr<Scene>> const& getAll();
		void add(std::unique_ptr<Scene>&&);
		void add(std::vector<std::unique_ptr<Scene>>&&);

	}

	inline std::vector<Shader>& shaders()
	{
		static std::vector<Shader> _shaders;
		return _shaders;
	}
	namespace textures
	{
		Texture& placeholder();
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

	enum ShaderType
	{
		unlit,
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
