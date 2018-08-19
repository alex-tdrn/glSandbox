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
		std::vector<std::shared_ptr<Mesh>> getAll();
		void add(std::shared_ptr<Mesh>);
		void add(std::vector<std::shared_ptr<Mesh>>&&);
		std::shared_ptr<Mesh> const& quad();
		std::shared_ptr<Mesh> const& box();

	}
	namespace scenes
	{
		std::vector<std::unique_ptr<Scene>> const& getAll();
		void add(std::unique_ptr<Scene>&&);
		void add(std::vector<std::unique_ptr<Scene>>&&);

	}

	inline std::vector<Shader> shaders;
	namespace textures
	{
		inline Texture placeholder("textures/placeholder.png", true);

	}
	namespace cubemaps
	{
		inline Cubemap skybox("skybox", {
			"cubemaps/skybox/right.jpg", "cubemaps/skybox/left.jpg",
			"cubemaps/skybox/top.jpg", "cubemaps/skybox/bottom.jpg",
			"cubemaps/skybox/front.jpg", "cubemaps/skybox/back.jpg"
			});
		inline Cubemap mp_blizzard("mp_blizzard", {
			"cubemaps/mp_blizzard/blizzard_rt.tga", "cubemaps/mp_blizzard/blizzard_lf.tga",
			"cubemaps/mp_blizzard/blizzard_up.tga", "cubemaps/mp_blizzard/blizzard_dn.tga",
			"cubemaps/mp_blizzard/blizzard_ft.tga", "cubemaps/mp_blizzard/blizzard_bk.tga"
			});

	}

	enum ShaderType
	{
		blinn_phong,
		phong,
		gouraud,
		flat,
		reflection,
		refraction,
		highlighting,
		debugNormals,
		debugNormalsShowLines,
		debugTexCoords,
		debugDepthBuffer,
		light,
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
