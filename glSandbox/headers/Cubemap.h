#pragma once
#include "AutoName.h"
#include "Texture.h"

#include <array>
#include <optional>

class Cubemap : public AutoName<Cubemap>
{
private:
	mutable bool allocated = false;
	mutable unsigned int ID;
	mutable int width = -1;
	mutable int height = -1;
	mutable int nrChannels = -1;
	mutable unsigned int format = 0;
	mutable unsigned int pixelTransfer = 0;
	mutable unsigned int dataType = 0;
	mutable bool mipmapping = true;
	mutable bool linear;
	std::optional<std::array<Texture, 6>> faces = std::nullopt;
	std::optional<Texture> equirectangularMap = std::nullopt;
	mutable Cubemap* convolutedMap = nullptr;

public:
	Cubemap() = delete;
	Cubemap(unsigned int format, int width, int height,
		unsigned int pixelTransfer, unsigned int dataType);
	Cubemap(std::array<Texture, 6>&& faces);
	Cubemap(Texture&& equirectangularMap);
	Cubemap(Cubemap const&) = delete;
	Cubemap(Cubemap&&) = default;
	Cubemap& operator= (Cubemap const&) = delete;
	Cubemap& operator= (Cubemap&&) = default;
	~Cubemap();

private:
	void allocate() const;
	void load() const;

protected:
	std::string getNamePrefix() const override;

public:
	Cubemap* convolute() const;
	unsigned int getID() const;
	void use(int location) const;
	void drawUI();

};