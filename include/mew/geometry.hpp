#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__ 1
#include "mew/Object.hpp"
#include "mew/GeometryShader.hpp"
namespace MEW {
	class Geometry : public Object {
	public:
		Geometry(std::vector<float>& vertices, Shader* shader);
		void DrawGeometry();
    void UseProgram();
	};
}

#endif //__OBJECT__