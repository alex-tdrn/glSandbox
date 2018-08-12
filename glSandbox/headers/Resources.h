#pragma once
#include "Shader.h"
#include "Model.h"
#include "Scene.h"
#include "Cubemap.h"

#include <vector>
#include <memory>
namespace resources
{
	namespace primitives
	{
		inline std::unique_ptr<Mesh> quad;
	}
	enum ShaderType
	{
		blinn_phong,
		phong,
		gouraud,
		flat,
		reflection,
		refraction,
		outline,
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
	inline unsigned int boxVAO;
	inline std::vector<Mesh> meshes;
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

	inline std::vector<std::shared_ptr<Scene>> scenes;
	inline std::shared_ptr<Scene> IOScene;
	void init();
	void reloadShaders();
	void drawUI(bool* open);
}
