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

Shader::Shader(std::string const vertexPath, std::string const fragmentPath)
	:vertexPath(vertexPath), fragmentPath(fragmentPath)
{
}

void Shader::reload()
{
	if(ID != -1)
		glDeleteProgram(ID);
	ID = glCreateProgram();
	std::string const vertexCode = read(vertexPath);
	std::string const fragmentCode = read(fragmentPath);
	if(vertexCode.empty() || fragmentCode.empty())
		throw("wtf");
	unsigned int vertex = compile(vertexCode, GL_VERTEX_SHADER);
	unsigned int fragment = compile(fragmentCode, GL_FRAGMENT_SHADER);
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
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
void Shader::set(std::string_view const name, glm::mat4 const& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(value));
}
