#pragma once
#include <string>

class AssetLoader
{
private:
	std::string filename;

public:
	AssetLoader() = delete;
	AssetLoader(std::string const& filename);


};