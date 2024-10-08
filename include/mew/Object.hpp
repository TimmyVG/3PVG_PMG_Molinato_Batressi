#ifndef __OBJECT_H__
#define __OBJECT_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "Shader.hpp"

namespace MEW {
	class Object {
		public:
			Object(std::vector<float>& vertices, Shader* shader);

			void Draw();
			void UseProgram();
	protected:
			std::vector<float> vertices_;
			Shader shader_;
			unsigned int vao_;

		private:
	};
}

#endif //__OBJECT__