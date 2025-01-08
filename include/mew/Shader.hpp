#ifndef __SHADER_H__
#define __SHADER_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "glm/mat4x4.hpp"

namespace MEW {
	class Shader {
		public:
			unsigned int vertexShader_;
			unsigned int fragmentShader_;
			unsigned int shaderProgram_;

			Shader();
			Shader(const char* vertexPath, const char* fragmentPath);
			void Draw(unsigned int vao);
			void UseProgram();
			void setBool(const char* name, bool value) const;
			void setInt(const char* name, int value) const;
			void setFloat(const char* name, float value) const;
			void setFloat3(const char* name,const float *value ) const;
			void setMat4(const char* name,glm::mat4x4 mat);

			void CompileProgram(std::vector<float>& vertices, unsigned int* vao);

			~Shader();
		private:
	};

};

#endif //__SHADER__