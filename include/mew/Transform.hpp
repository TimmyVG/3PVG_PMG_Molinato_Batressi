#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__ 1
#include <vector>
#include <string>
#include <mew/Mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace MEW {
	struct TransformComponent {
		glm::mat4x4 mat_;
		glm::vec3 scale_;
		glm::vec3 rotation_;
		glm::vec3 translation_;
		TransformComponent();
	};

	class TransformSystem {
	public:
		void SetTranslation(glm::vec3 pos, TransformComponent* tc);
		void Translate(glm::vec3 pos, TransformComponent* tc);
		void TranslateX(float pos, TransformComponent* tc);
		void TranslateY(float pos, TransformComponent* tc);
		void TranslateZ(float pos, TransformComponent* tc);
		void SetRotation(glm::vec3 rot, TransformComponent* tc);
		void Rotate(glm::vec3 rot, TransformComponent* tc);
		void RotateX(float rot, TransformComponent* tc);
		void RotateY(float rot, TransformComponent* tc);
		void RotateZ(float rot, TransformComponent* tc);
		void SetScale(glm::vec3 scale, TransformComponent* tc);
		void Scale(glm::vec3 scale, TransformComponent* tc);
		void ScaleX(float scale, TransformComponent* tc);
		void ScaleY(float scale, TransformComponent* tc);
		void ScaleZ(float scale, TransformComponent* tc);
		glm::vec3 GetScale(TransformComponent* tc);
		glm::vec3 GetRotation(TransformComponent* tc);
		glm::vec3 GetTranslation(TransformComponent* tc);


	private:
	};


}

#endif //__TRANSFORM__