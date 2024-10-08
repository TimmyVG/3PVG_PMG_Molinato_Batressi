#include "mew/Shader.hpp"
#include "GL/glew.h"
#include <iostream>

MEW::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"out vec3 vertexColor;\n"
		"uniform vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   vertexColor = ourColor;\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 vertexColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(vertexColor, 1.0f);\n"
		"}\n\0";



	const char* vShaderCode;
	const char* fShaderCode;
	//leer los dos fcheros

	glewInit();

	GLuint vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error al crear el shader: " << error << std::endl;
	}
	glShaderSource(vertexShader_, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader_);

	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader_);

	shaderProgram_ = glCreateProgram();

	glAttachShader(shaderProgram_, vertexShader_);
	glAttachShader(shaderProgram_, fragmentShader_);
	glLinkProgram(shaderProgram_);

	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
}

void MEW::Shader::Draw(unsigned int vao) {
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


MEW::Shader::~Shader()
{
	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
}

void MEW::Shader::CompileProgram(std::vector<float>& vertices, unsigned int* vao)
{
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()  * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
