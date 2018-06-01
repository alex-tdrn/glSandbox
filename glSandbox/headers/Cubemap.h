#pragma once
#include <array>
#include <string>

class Cubemap
{
private:
	unsigned int ID;
	std::array<std::string, 6> paths;
	std::string name;
	bool initialized = false;
	int width, height, nrChannels;

public:
	Cubemap(std::string name, std::array<std::string, 6> paths);

public:
	std::string_view getName() const;
	void use();
	void initialize();
	[[nodiscard]] bool drawUI();

};