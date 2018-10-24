#include "MaterialManager.h"
#include "TextureManager.h"
#include "MaterialPBRMetallicRoughness.h"

void MaterialManager::initialize()
{
	uvChecker();
}

Material* MaterialManager::uvChecker()
{
	static auto ret = [&]() -> Material*{
		auto material = std::make_unique<MaterialPBRMetallicRoughness>();
		material->setBaseColor(TextureManager::uvChecker3M(), std::nullopt);
		material->setName("UV Checker");
		return add(std::move(material));
	}();
	return ret;
}
