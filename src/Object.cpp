#include "mew/Object.hpp"
#include "GL/glew.h"
MEW::Object::Object(std::vector<float>& vertices, Shader* shader)
{
	vertices_ = vertices;
	shader_ = *shader;
	shader->CompileProgram(vertices_,&vao_);
}


void MEW::Object::Draw()
{
	shader_.Draw(vao_);
}

void MEW::Object::UseProgram() {
	shader_.UseProgram();
}
