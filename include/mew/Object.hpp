#ifndef __OBJECT_H__
#define __OBJECT_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "glm/mat4x4.hpp"
#include "Mesh.hpp"
#include "Model.hpp"

namespace MEW {
	class Object {
		public:
			Object(std::string, Shader* shader);

			void Draw();
			void UseProgram();
			void SetTranslation(glm::vec3 pos);
			void Translate(glm::vec3 pos);
			void TranslateX(float pos);
			void TranslateY(float pos);
			void TranslateZ(float pos);
			void SetRotation(glm::vec3 rot);
			void Rotate(glm::vec3 rot);
			void RotateX(float rot);
			void RotateY(float rot);
			void RotateZ(float rot);
			void SetScale(glm::vec3 scale);
			void Scale(glm::vec3 scale);
			void ScaleX(float scale);
			void ScaleY(float scale);
			void ScaleZ(float scale);
	protected:
			Model* model;
			std::vector<float> vertices_;
			unsigned int actMesh;
			Shader* shader_;
			glm::mat4x4 mat_;
			glm::vec3 scale_;
			glm::vec3 rotation_;
			glm::vec3 translation_;
			

		private:
			std::vector<Mesh> meshes_;
	};
}

#endif //__OBJECT__