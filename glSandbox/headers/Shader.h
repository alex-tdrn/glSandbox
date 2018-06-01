#pragma once
#include <glm\gtc\matrix_transform.hpp>
#include <glad\glad.h>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
private:
	unsigned int ID = -1;
	std::string const vertexPath;
	std::string const fragmentPath;

public:
	Shader(std::string const vertexPath, std::string const fragmentPath);

public:
	void reload();
	void use();
	void set(std::string_view const name, int value) const;
	void set(std::string_view const name, float value) const;
	void set(std::string_view const name, glm::vec2 const& value) const;
	void set(std::string_view const name, glm::vec3 const& value) const;
	void set(std::string_view const name, glm::mat4 const& value) const;
};