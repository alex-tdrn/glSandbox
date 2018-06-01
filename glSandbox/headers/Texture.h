#pragma once
#include <string_view>
#include <glad\glad.h>
#include <atomic>
#include <glm/glm.hpp>

class Texture
{
private:
	std::atomic<bool>* ready = new std::atomic<bool>(false);
	mutable bool initialized = false;
	unsigned int ID;
	glm::vec2 uvOffset{0, 0};
	struct ImageData
	{
		unsigned char* data = nullptr;
		int width = 0;
		int height = 0;
		int nrChannels = 0;
	}* image = nullptr;
	std::string path;

public:
	explicit Texture(std::string const path);
	Texture(Texture const& other);
	~Texture();
	Texture const & operator=(Texture const& other);

public:
	glm::vec2 const& getUVOffset() const;
	bool isInitialized() const;
	std::string_view getPath() const;
	void use(GLenum location) const;
	[[nodiscard]]bool drawUI();

};
