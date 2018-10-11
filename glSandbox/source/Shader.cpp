#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

std::string read(std::string_view const path)
{
	std::ifstream file(path.data());
	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

unsigned int compile(std::string_view const source, GLenum type)
{
	unsigned int compiledShader = glCreateShader(type);
	char const* stupidfuckingbullshit = source.data();
	glShaderSource(compiledShader, 1, &stupidfuckingbullshit, nullptr);
	glCompileShader(compiledShader);
	return compiledShader;
}

Shader::Shader(std::string const vertexPath, std::string const fragmentPath, std::optional<std::string const> geometryPath)
	:vertexPath(vertexPath), fragmentPath(fragmentPath), geometryPath(geometryPath)
{
}

void Shader::reload()
{
	if(initialized)
		glDeleteProgram(ID);
	initialized = true;
	ID = glCreateProgram();

	std::string const vertexCode = read(vertexPath);
	unsigned int vertex = compile(vertexCode, GL_VERTEX_SHADER);
	glAttachShader(ID, vertex);

	std::string const fragmentCode = read(fragmentPath);
	unsigned int fragment = compile(fragmentCode, GL_FRAGMENT_SHADER);
	glAttachShader(ID, fragment);

	unsigned int geometry;
	if(geometryPath)
	{
		std::string const geometryCode = read(*geometryPath);
		geometry = compile(geometryCode, GL_GEOMETRY_SHADER);
		glAttachShader(ID, geometry);
	}

	glLinkProgram(ID);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if(geometryPath)
		glDeleteShader(geometry);
}

void Shader::use()
{
	if(!initialized)
		reload();
	glUseProgram(ID);
}

void Shader::set(std::string_view const name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.data()), value);
}
void Shader::set(std::string_view const name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.data()), value);
}
void Shader::set(std::string_view const name, glm::vec2 const& value) const
{
	glUniform2f(glGetUniformLocation(ID, name.data()), value.x, value.y);
}
void Shader::set(std::string_view const name, glm::vec3 const& value) const
{
	glUniform3f(glGetUniformLocation(ID, name.data()), value.x, value.y, value.z);
}
void Shader::set(std::string_view const name, glm::vec4 const& value) const
{
	glUniform4f(glGetUniformLocation(ID, name.data()), value.x, value.y, value.z, value.w);
}
void Shader::set(std::string_view const name, glm::mat4 const& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::drawUI()
{
	//TODO
}
