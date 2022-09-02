#include "Shader.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

Shader::Shader(unsigned int id) : id(id){
}

Shader::~Shader(){
	glDeleteProgram(id);
}

[[maybe_unused]] void Shader::use() const {
	glUseProgram(id);
}

void Shader::uniformMatrix(const std::string& name, glm::mat4 matrix) const{
	const GLuint transformLoc = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(static_cast<GLint>(transformLoc), 1, GL_FALSE, glm::value_ptr(matrix));
}

[[maybe_unused]] void Shader::uniform1i(const std::string& name, int x) const{
	const GLuint transformLoc = glGetUniformLocation(id, name.c_str());
	glUniform1i(static_cast<GLint>(transformLoc), x);
}

void Shader::uniform1f(const std::string& name, float x) const{
	const GLuint transformLoc = glGetUniformLocation(id, name.c_str());
	glUniform1f(static_cast<GLint>(transformLoc), x);
}

[[maybe_unused]] void Shader::uniform2f(const std::string& name, float x, float y) const{
	const GLuint transformLoc = glGetUniformLocation(id, name.c_str());
	glUniform2f(static_cast<GLint>(transformLoc), x, y);
}

void Shader::uniform3f(const std::string& name, float x, float y, float z) const{
	const GLuint transformLoc = glGetUniformLocation(id, name.c_str());
	glUniform3f(static_cast<GLint>(transformLoc), x,y,z);
}

Shader* load_shader(const std::string& vertexFile, const std::string& fragmentFile) {
	// Reading Files
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		vShaderFile.open(vertexFile);
		fShaderFile.open(fragmentFile);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure& e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		return nullptr;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		std::cerr << "SHADER::VERTEX: compilation failed" << std::endl;
		std::cerr << infoLog << std::endl;
		return nullptr;
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		std::cerr << "SHADER::FRAGMENT: compilation failed" << std::endl;
		std::cerr << infoLog << std::endl;
		return nullptr;
	}

	// Shader Program
	GLuint id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		std::cerr << "SHADER::PROGRAM: linking failed" << std::endl;
		std::cerr << infoLog << std::endl;

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return nullptr;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return new Shader(id);
}
