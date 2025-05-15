#ifndef __OBJECT_H__
#define __OBJECT_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "glm/mat4x4.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "mew/Transform.hpp"
namespace MEW {
	class Object {
		public:
			Object();
			Object(std::string, Shader* shader);
			Object(Shader* shader);

			void Draw();
			void UseProgram();
			Model* model;
			size_t entity_;
			Shader* shader_;
			unsigned int vao_;
	protected:
			std::vector<float> vertices_;
			unsigned int actMesh;

			

		private:
			std::vector<Mesh> meshes_;
	};
}

#endif //__OBJECT__