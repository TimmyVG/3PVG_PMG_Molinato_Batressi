#ifndef __MESH_H__
#define __MESH_H__ 1
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "TextureTest.hpp"

namespace MEW {

  struct Vertex_Data {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  struct MeshData {
    std::vector<Vertex_Data> vertexs_;
    std::vector<unsigned int> ids_;
    std::optional<TextureData> diffuse_tex_data;
    std::optional<TextureData> normal_tex_data;
    std::optional<TextureData> specular_tex_data;
    float minX, maxX, minY, maxY, minZ, maxZ;
  };

	class Mesh {
	public:
		std::vector<unsigned int> indices_;

		std::optional<Texture> diffuse_tex_;
		std::optional<Texture> normal_tex_;
		std::optional<Texture> specular_tex_;
		std::optional<Texture> displacement_tex;

		Mesh(const MeshData& other_mesh_data);
		~Mesh();
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh& other);
		Mesh(Mesh& other);
		void setUpMesh(const MeshData& mesh_data);
		const unsigned int GetVAO() const;
		
	private:
		unsigned int VAO, VBO, EBO;
		bool isMeshLoaded;
	};

	MeshData generateGridMesh(int width, int height , int resolutionX , int resolutionY);
}

#endif