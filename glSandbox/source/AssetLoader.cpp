#include "AssetLoader.h"

#include <fx/gltf.h>
using namespace fx;
AssetLoader::AssetLoader(std::string const& filename)
	: filename(filename)
{
	gltf::Document doc = gltf::LoadFromText(filename);

}