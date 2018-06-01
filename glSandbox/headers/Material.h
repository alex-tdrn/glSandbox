#pragma once
#include "Texture.h"
#include "Shader.h"

#include <string>
#include <optional>
#include <array>

class Material
{
public:
	enum Maps
	{
		ambient,
		diffuse,
		specular,
		shininess,
		emission,
		light,
		reflection,
		opacity,
		normal,
		bump,
		displacement,
		unknown,
		n
	};
private:
	std::string const name;
	std::array<std::optional<Texture>, Maps::n> maps;
	float shininessValue = 16.0f;
	inline static int ct = 0;

public:
	Material(std::string const name = "Material#" + std::to_string(ct++));

public:
	void setMap(int mapType, std::optional<Texture> map);
	bool isInitialized() const;
	std::string_view const getName() const;
	bool contains(std::string_view const path);
	void use(Shader shader) const;
	[[nodiscard]]bool drawUI();
};