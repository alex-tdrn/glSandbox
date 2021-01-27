#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <imgui.h>

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
	int logLength;

	glGetShaderiv(compiledShader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		std::string log;
		log.resize(logLength + 1);
		glGetShaderInfoLog(compiledShader, logLength, NULL, log.data());
		std::cout << "ERROR COMPILING SHADER" << ":\n" << log;
		assert(false);
	}

	return compiledShader;
}

Shader::Shader(std::string const vertexPath, std::string const fragmentPath, std::optional<std::string const> geometryPath)
	:vertexPath(vertexPath), fragmentPath(fragmentPath), geometryPath(geometryPath)
{
}

int Shader::getLocation(std::string_view const name) const
{
	int res = glGetUniformLocation(ID, name.data());
//#ifndef NDEBUG
//	assert(res != -1);
//#endif
	return res;
}

std::string Shader::getNamePrefix() const
{
	return "shader";
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

	int logLength;
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		std::string log;
		log.resize(logLength + 1);
		glGetProgramInfoLog(ID, logLength, NULL, log.data());
		std::cout << "ERROR LINKING: \n" << log;
		//assert(false);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if(geometryPath)
		glDeleteShader(geometry);

	//validate();
}

void Shader::use()
{
	if(!initialized)
		reload();
	glUseProgram(ID);
}

void Shader::validate()
{
	glValidateProgram(ID);
	int logLength;
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		std::string log;
		log.resize(logLength + 1);
		glGetProgramInfoLog(ID, logLength, NULL, log.data());
		std::cout << "Shader validation complete\n";
		std::cout << "    Vertex path: " << vertexPath << '\n';
		std::cout << "    Fragment path: " << fragmentPath << '\n';
		std::cout << "Validation result: \n" << log;
		//assert(false);
	}
}

void Shader::set(std::string_view const name, int value) const
{
	glUniform1i(getLocation(name), value);
}
void Shader::set(std::string_view const name, float value) const
{
	glUniform1f(getLocation(name), value);
}
void Shader::set(std::string_view const name, glm::vec2 const& value) const
{
	glUniform2f(getLocation(name), value.x, value.y);
}
void Shader::set(std::string_view const name, glm::vec3 const& value) const
{
	glUniform3f(getLocation(name), value.x, value.y, value.z);
}
void Shader::set(std::string_view const name, glm::vec4 const& value) const
{
	glUniform4f(getLocation(name), value.x, value.y, value.z, value.w);
}
void Shader::set(std::string_view const name, glm::mat4 const& value) const
{
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::drawUI()
{
	if(ImGui::Button("Reload"))
		reload();
}
