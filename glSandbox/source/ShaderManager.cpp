#include "ShaderManager.h"

void ShaderManager::initialize()
{
	visualizeTexture();
	visualizeMesh();
	visualizeCubemap();
	unlit();
	pbr();
	blinnPhong();
	phong();
	gouraud();
	flat();
	reflection();
	refraction();
	equirectangularToCubemap();
	convoluteCubemap();
	debugNormals();
	debugNormalsShowLines();
	debugTexCoords();
	debugDepthBuffer();
	shadowMappingUnidirectional();
	shadowMappingOmnidirectional();
	skybox();
	gammaHDR();
	passthrough();
	grayscale();
	chromaticAberration();
	invert();
	convolution();
}

void ShaderManager::reloadAll()
{
	for(auto& shader : getAll())
		shader->reload();
}

bool ShaderManager::isLightingShader(Shader* shader)
{
	return
		shader == pbr() ||
		shader == blinnPhong() ||
		shader == phong() ||
		shader == gouraud() ||
		shader == flat();
}

template <typename ...Sources>
Shader* load(std::string&& name, Sources&&... sources)
{
	auto shader = std::make_unique<Shader>(sources...);
	shader->setName(std::move(name));
	return ShaderManager::add(std::move(shader));
}

Shader* ShaderManager::visualizeTexture()
{
	static auto ret = load("Visualize Texture",
		"shaders/visualizeTexture.vert", "shaders/visualizeTexture.frag"
	);
	return ret;
}

Shader* ShaderManager::visualizeMesh()
{
	static auto ret = load("Visualize Mesh",
		"shaders/visualizeMesh.vert", "shaders/visualizeMesh.frag"
	);
	return ret;
}

Shader* ShaderManager::visualizeCubemap()
{
	static auto ret = load("Visualize Cubemap",
		"shaders/visualizeCubemap.vert", "shaders/visualizeCubemap.frag"
	);
	return ret;
}

Shader* ShaderManager::unlit()
{
	static auto ret = load("Unlit",
		"shaders/unlit.vert", "shaders/unlit.frag"
	);
	return ret;
}

Shader* ShaderManager::pbr()
{
	static auto ret = load("PBR",
		"shaders/pbr.vert", "shaders/pbr.frag"
	);
	return ret;
}

Shader* ShaderManager::blinnPhong()
{
	static auto ret = load("Blinn-Phong",
		"shaders/phong.vert", "shaders/blinn-phong.frag"
	);
	return ret;
}

Shader* ShaderManager::phong()
{
	static auto ret = load("Phong",
		"shaders/phong.vert", "shaders/phong.frag"
	);
	return ret;
}

Shader* ShaderManager::gouraud()
{
	static auto ret = load("Gouraud",
		"shaders/gouraud.vert", "shaders/gouraud.frag"
	);
	return ret;
}

Shader* ShaderManager::flat()
{
	static auto ret = load("Flat",
		"shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom"
	);
	return ret;
}

Shader* ShaderManager::reflection()
{
	static auto ret = load("Reflection",
		"shaders/reflection.vert", "shaders/reflection.frag"
	);
	return ret;
}

Shader* ShaderManager::refraction()
{
	static auto ret = load("Refraction",
		"shaders/refraction.vert", "shaders/refraction.frag"
	);
	return ret;
}

Shader* ShaderManager::equirectangularToCubemap()
{
	static auto ret = load("Equirectangular Conversion",
		"shaders/equirectangularToCubemap.vert", "shaders/equirectangularToCubemap.frag", "shaders/equirectangularToCubemap.geom"
	);
	return ret;
}

Shader* ShaderManager::convoluteCubemap()
{
	static auto ret = load("Equirectangular Conversion",
		"shaders/convoluteCubemap.vert", "shaders/convoluteCubemap.frag", "shaders/convoluteCubemap.geom"
	);
	return ret;
}

Shader* ShaderManager::debugNormals()
{
	static auto ret = load("Debug Normals",
		"shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom"
	);
	return ret;
}

Shader* ShaderManager::debugNormalsShowLines()
{
	static auto ret = load("Debug Normals Show Lines",
		"shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom"
	);
	return ret;
}

Shader* ShaderManager::debugTexCoords()
{
	static auto ret = load("Debug Texture Coordinates",
		"shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag"
	);
	return ret;
}

Shader* ShaderManager::debugDepthBuffer()
{
	static auto ret = load("Debug Depth Buffer",
		"shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag"
	);
	return ret;
}

Shader* ShaderManager::shadowMappingUnidirectional()
{
	static auto ret = load("Shadow Mapping Unidirectional",
		"shaders/shadowMappingUnidirectional.vert", "shaders/shadowMappingUnidirectional.frag"
	);
	return ret;
}

Shader* ShaderManager::shadowMappingOmnidirectional()
{
	static auto ret = load("Shadow Mapping Omnidirectional",
		"shaders/shadowMappingOmnidirectional.vert", "shaders/shadowMappingOmnidirectional.frag", "shaders/shadowMappingOmnidirectional.geom"
	);
	return ret;
}

Shader* ShaderManager::skybox()
{
	static auto ret = load("Skybox",
		"shaders/skybox.vert", "shaders/skybox.frag"
	);
	return ret;
}

Shader* ShaderManager::gammaHDR()
{
	static auto ret = load("Gamma / HDR",
		"shaders/pp.vert", "shaders/ppGammaHDR.frag"
	);
	return ret;
}

Shader* ShaderManager::passthrough()
{
	static auto ret = load("Passthrough",
		"shaders/pp.vert", "shaders/ppPassthrough.frag"
	);
	return ret;
}

Shader* ShaderManager::grayscale()
{
	static auto ret = load("Grayscale",
		"shaders/pp.vert", "shaders/ppBW.frag"
	);
	return ret;
}

Shader* ShaderManager::chromaticAberration()
{
	static auto ret = load("Chromatic Aberration",
		"shaders/pp.vert", "shaders/ppChromaticAberration.frag"
	);
	return ret;
}

Shader* ShaderManager::invert()
{
	static auto ret = load("Invert",
		"shaders/pp.vert", "shaders/ppInvert.frag"
	);
	return ret;
}

Shader* ShaderManager::convolution()
{
	static auto ret = load("Convolution",
		"shaders/pp.vert", "shaders/ppConvolution.frag"
	);
	return ret;
}
