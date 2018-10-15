#include "ShaderManager.h"

void ShaderManager::initialize()
{
	unlit();
	pbr();
	blinnPhong();
	phong();
	gouraud();
	flat();
	reflection();
	refraction();
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

Shader* ShaderManager::unlit()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/unlit.vert", "shaders/unlit.frag");
		shader->name.set("Unlit");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::pbr()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbr.frag");
		shader->name.set("PBR");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::blinnPhong()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/phong.vert", "shaders/blinn-phong.frag");
		shader->name.set("Blinn-Phong");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::phong()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/phong.vert", "shaders/phong.frag");
		shader->name.set("Phong");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::gouraud()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/gouraud.vert", "shaders/gouraud.frag");
		shader->name.set("Gouraud");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::flat()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
		shader->name.set("Flat");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::reflection()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/reflection.vert", "shaders/reflection.frag");
		shader->name.set("Reflection");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::refraction()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/refraction.vert", "shaders/refraction.frag");
		shader->name.set("Refraction");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::debugNormals()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
		shader->name.set("Debug Normals");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::debugNormalsShowLines()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
		shader->name.set("Debug Normals Show Lines");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::debugTexCoords()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
		shader->name.set("Debug Texture Coordinates");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::debugDepthBuffer()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
		shader->name.set("Debug Depth Buffer");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::shadowMappingUnidirectional()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/shadowMappingUnidirectional.vert", "shaders/shadowMappingUnidirectional.frag");
		shader->name.set("Shadow Mapping Unidirectional");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::shadowMappingOmnidirectional()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/shadowMappingOmnidirectional.vert", "shaders/shadowMappingOmnidirectional.frag", "shaders/shadowMappingOmnidirectional.geom");
		shader->name.set("Shadow Mapping Omnidirectional");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::skybox()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
		shader->name.set("Skybox");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::gammaHDR()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppGammaHDR.frag");
		shader->name.set("Gamma / HDR");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::passthrough()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppPassthrough.frag");
		shader->name.set("Passthrough");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::grayscale()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppBW.frag");
		shader->name.set("Grayscale");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::chromaticAberration()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
		shader->name.set("Chromatic Aberration");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::invert()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppInvert.frag");
		shader->name.set("Invert");
		return add(std::move(shader));
	}();
	return ret;
}

Shader* ShaderManager::convolution()
{
	static auto ret = [&]() -> Shader*{
		auto shader = std::make_unique<Shader>("shaders/pp.vert", "shaders/ppConvolution.frag");
		shader->name.set("Convolution");
		return add(std::move(shader));
	}();
	return ret;
}
