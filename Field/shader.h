#pragma once

#include <GL/glew.h>

class Shader {
public:
	GLuint Program;
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	void Use();
private:
	void compileShader(GLuint id, const GLchar* filepath);
};

/*
void compileShader(GLuint id, std::string filepath) {
	// Concatenate file to string
	std::string content;
	std::ifstream file(filepath, std::ios::in);
	if (!file.is_open()) {
	std::cout << "Could not read file " << filepath << ". File does not exist.\n";
	return;
	}
	std::string line = "";
	while (!file.eof()) {
	std::getline(file, line);
	content.append(line + "\n");
	}
	file.close();
	// Compile
	const char* src = content.c_str();
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
*/