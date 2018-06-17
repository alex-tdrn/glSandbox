#pragma once
#include <string_view>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <future>

class Texture
{
private:
	mutable bool initialized = false;
	bool linear;
	mutable unsigned int ID;
	glm::vec2 uvOffset{0, 0};
	struct ImageData
	{
		unsigned char* data = nullptr;
		int width = 0;
		int height = 0;
		int nrChannels = 0;
	};
	mutable ImageData image;
	mutable std::future<ImageData> loader;
	std::string path;

public:
	Texture() = delete;
	Texture(std::string const path, bool linear);
	Texture(Texture const& other) = delete;
	Texture(Texture&& other) = default;
	Texture& operator=(Texture const& other) = delete;
	Texture& operator=(Texture&& other) = default;

private:
	void initialize(int location) const;

public:
	glm::vec2 const getUVOffset() const;
	bool isLoaded() const;
	std::string_view getPath() const;
	void use(int location) const;
	[[nodiscard]]bool drawUI();

};
