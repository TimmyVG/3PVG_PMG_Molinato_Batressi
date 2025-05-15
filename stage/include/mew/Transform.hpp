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
		glm::mat4 model;
		TransformComponent();

	};


	class TransformSystemMat {
	public:
		void operator()(std::vector<std::optional<MEW::TransformComponent>>& vecTransform);
	};
}

#endif //__TRANSFORM__