#pragma once
#include "glad/glad.h"
#include "AutoName.h"

#include <string>
#include <glm/glm.hpp>
#include <optional>

class Texture : public AutoName<Texture>
{
	friend class Cubemap;
private:
	mutable bool allocated = false;
	mutable unsigned int ID;
	mutable int width = -1;
	mutable int height = -1;
	mutable int nrChannels = -1;
	mutable unsigned int format = 0;
	mutable unsigned int pixelTransfer = 0;
	mutable unsigned int dataType = 0;
	bool mipmapping = true;
	bool linear = true;
	bool hdr = false;
	std::optional<std::string> path = std::nullopt;

public:
	Texture() = delete;
	Texture(unsigned int format, int width, int height,
		unsigned int pixelTransfer, unsigned int dataType,
		void* imageData = nullptr);
	Texture(std::string const& path, bool linear);
	Texture(Texture const& other) = delete;
	Texture(Texture&& other);
	Texture& operator=(Texture const& other) = delete;
	Texture& operator=(Texture&& other);
	~Texture();

private:
	void allocate(void* imageData = nullptr) const;
	void load() const;

protected:
	std::string getNamePrefix() const override;

public:
	unsigned int getID() const;
	int getWidth() const;
	int getHeight() const;
	int getNumberOfChannels() const;
	void use(int location) const;
	bool isLinear() const;
	void drawUI();

};
