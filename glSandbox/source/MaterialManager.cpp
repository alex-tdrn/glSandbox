#include "MaterialManager.h"
#include "TextureManager.h"
#include "MaterialPBRMetallicRoughness.h"

void MaterialManager::initialize()
{
	basic();
}

Material* MaterialManager::basic()
{
	static auto ret = [&]() -> Material*{
		auto material = std::make_unique<MaterialPBRMetallicRoughness>();
		material->setBaseColor(TextureManager::debug(), std::nullopt);
		material->name.set("basic");
		return add(std::move(material));
	}();
	return ret;
}
