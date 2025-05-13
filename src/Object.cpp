#include "mew/Object.hpp"

#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>

namespace MEW {
	Object::Object() {

	}
	Object::Object(std::string path, Shader* shader)
	{
		shader_ = shader;
		actMesh = 0;
		//Load Meshes
	}

	Object::Object(Shader* shader)
	{
		shader_ = shader;
		actMesh = 0;
		//Load Meshes
	}



	void Object::UseProgram() {
		shader_->UseProgram();
	}
}
