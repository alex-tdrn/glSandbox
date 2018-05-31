#pragma once
#include "Texture.h"
#include "Shader.h"

#include <string>
#include <optional>

class Material
{
private:
	std::string const name;
	std::optional<Texture> const diffuseMap;
	std::optional<Texture> const specularMap;
	float shininess;
	static int ct;

public:
	Material(std::optional<Texture> diffuseMap, std::optional<Texture> specularMap, float shininess = 32.0f, std::string const name = "Material#" + std::to_string(ct++));

public:
	bool isInitialized() const;
	std::string_view const getName() const;
	bool contains(std::string_view const path);
	void use(Shader shader) const;
	void drawUI();
};