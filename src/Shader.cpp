#include "mew/Shader.hpp"
#include "GL/glew.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

MEW::Shader::Shader()
{
	fragmentShader_ = 0;
	vertexShader_ = 0;
	shaderProgram_ = 0;
}

MEW::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	fragmentShader_ = 0;
	vertexShader_ = 0;
	shaderProgram_ = 0;
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// open files
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;
	// read file's buffer contents into streams
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	// close file handlers
	vShaderFile.close();
	fShaderFile.close();
	// convert stream into string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	//leer los dos fcheros

	glewInit();

	GLuint vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error al crear el shader: " << error << std::endl;
	}
	glShaderSource(vertexShader_, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader_);

	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader_);

	shaderProgram_ = glCreateProgram();

	glAttachShader(shaderProgram_, vertexShader_);
	glAttachShader(shaderProgram_, fragmentShader_);
	glLinkProgram(shaderProgram_);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
		std::cout << "ERROR: Falló la vinculación del Shader Program:\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);

}
MEW::Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	fragmentShader_ = 0;
	vertexShader_ = 0;
	geometryShader_ = 0;
	shaderProgram_ = 0;
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// open files
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	gShaderFile.open(geometryPath);
	std::stringstream vShaderStream, fShaderStream, gShaderStream;
	// read file's buffer contents into streams
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	gShaderStream << gShaderFile.rdbuf();
	// close file handlers
	vShaderFile.close();
	fShaderFile.close();
	gShaderFile.close();
	// convert stream into string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();
	geometryCode = gShaderStream.str();
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	//leer los dos fcheros

	glewInit();

	vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error al crear el shader: " << error << std::endl;
	}
	glShaderSource(vertexShader_, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader_);

	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader_);

	geometryShader_ = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader_, 1, &gShaderCode, NULL);
	glCompileShader(geometryShader_);

	shaderProgram_ = glCreateProgram();

	glAttachShader(shaderProgram_, vertexShader_);
	glAttachShader(shaderProgram_, fragmentShader_);
	glAttachShader(shaderProgram_, geometryShader_);
	glLinkProgram(shaderProgram_);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
		std::cout << "ERROR: Falló la vinculación del Shader Program:\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
	glDeleteShader(geometryShader_);

}


void MEW::Shader::Draw(unsigned int vao)
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}


void MEW::Shader::UseProgram()
{
	glUseProgram(shaderProgram_);
}

void MEW::Shader::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram_, name), (int)value);
}

void MEW::Shader::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram_, name), value);
}


void MEW::Shader::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderProgram_, name), value);
}
void MEW::Shader::setFloat3(const char* name, const float *value) const
{
	glUniform3f(glGetUniformLocation(shaderProgram_, name), value[0], value[1], value[2]);
}

void MEW::Shader::setMat4(const char* name, glm::mat4x4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, name), 1,GL_FALSE, glm::value_ptr(mat));

}

void MEW::Shader::CompileProgram(std::vector<float>& vertices, unsigned int* vao)
{
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}


MEW::Shader::~Shader()
{
	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
}

