#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : Shader(vertexPath, nullptr, fragmentPath) {}

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath) {
	this->Program = glCreateProgram();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertex, vertexPath);
	glAttachShader(this->Program, vertex);

	if (geometryPath) {
		GLuint geometry = glCreateShader(GL_GEOMETRY_SHADER);
		compileShader(geometry, geometryPath);
		glAttachShader(this->Program, geometry);
	}

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragment, fragmentPath);
	glAttachShader(this->Program, fragment);

	glLinkProgram(this->Program);

	// Check
	GLint success;
	GLchar infolog[512];
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, NULL, infolog);
		std::cout << "Failed to link program\n" << infolog << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::compileShader(GLuint id, const GLchar* filepath)
{
	// Concatenate file to string
	std::string code;
	std::ifstream file(filepath, std::ios::in);
	if (!file.is_open()) {
		std::cout << "Could not read file " << filepath << ". File does not exist.\n";
		return;
	}
	std::string line = "";
	while (!file.eof()) {
		std::getline(file, line);
		code.append(line + "\n");
	}
	file.close();

	// Compile
	const GLchar* src = code.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Check
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "Shader " << filepath << " failed to load!" << infoLog << std::endl;
	}
}

void Shader::Use() {
	glUseProgram(this->Program);
}