#pragma once
#include "Named.h"
#include "Texture.h"

#include <array>
#include <optional>

class Cubemap
{
private:
	mutable bool loaded = false;
	mutable unsigned int ID;
	std::array<Texture, 6> faces;

public:
	Name<Cubemap> name{"cubemap"};

public:
	Cubemap() = delete;
	Cubemap(std::array<Texture, 6>&& faces);
	Cubemap(Cubemap const&) = delete;
	Cubemap(Cubemap&&) = default;
	Cubemap& operator= (Cubemap const&) = delete;
	Cubemap& operator= (Cubemap&&) = default;
	~Cubemap() = default;

private:
	void load();

public:
	void use(int location);
	void drawUI();

};