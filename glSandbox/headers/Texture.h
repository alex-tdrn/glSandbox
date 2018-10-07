#pragma once
#include "Named.h"

#include <string>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <optional>

class Texture
{
private:
	mutable bool loaded = false;
	bool linear;
	mutable unsigned int ID;
	mutable int width = 0;
	mutable int height = 0;
	mutable int nrChannels = 0;
	std::optional<std::string> path = std::nullopt;

public:
	Name<Texture> name{"texture"};

public:
	Texture() = delete;
	Texture(std::string const& path, bool linear);
	Texture(Texture const& other) = delete;
	Texture(Texture&& other) = default;
	Texture& operator=(Texture const& other) = delete;
	Texture& operator=(Texture&& other) = default;

private:
	void load() const;

public:
	void use(int location) const;
	void drawUI();

};
