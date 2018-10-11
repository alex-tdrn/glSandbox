#pragma once
#include "glad/glad.h"
#include "Named.h"

#include <string>
#include <glm/glm.hpp>
#include <optional>

class Texture
{
	friend class Cubemap;
private:
	mutable bool loaded = false;
	mutable unsigned int ID;
	mutable int width = -1;
	mutable int height = -1;
	mutable int nrChannels = -1;
	mutable unsigned int format = 0;
	mutable unsigned int pixelTransfer = 0;
	bool linear;
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
	~Texture();

private:
	void load() const;

public:
	void use(int location) const;
	void drawUI();

};
