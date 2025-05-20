#include "mew/Model.hpp"
#include <iostream>
#include <GL/glew.h>
#include <gl/GL.h>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <unordered_map>

namespace MEW {
	Model::Model(std::vector<MeshData>& mesh_data) {
		setUpData(mesh_data);
	}
	std::string GetDataHash(unsigned char* data, size_t size) {
		std::hash<std::string> hasher;
		return std::to_string(hasher(std::string((char*)data, size)));
	}

	void Model::setUpData(std::vector<MeshData>& mesh_data) {

		for (int i = 0; i < mesh_data.size(); i++) {
			Mesh mesh = Mesh(mesh_data[i]);
			TextureData* texturedata;
			if (mesh_data[i].diffuse_tex_data) {
				texturedata = &mesh_data[i].diffuse_tex_data.value();

				const std::string& key = texturedata->cadena;

				auto it = loaded_tex_map.find(key);
				if (it != loaded_tex_map.end()) {
					mesh.diffuse_tex_ = *(it->second);
				}
				else {
					std::shared_ptr<Texture> texPtr = std::make_shared<Texture>(*texturedata);
					loaded_tex_map[key] = texPtr;
					mesh.diffuse_tex_ = *texPtr;
				}
			}

			if (mesh_data[i].normal_tex_data) {
				texturedata = &mesh_data[i].normal_tex_data.value();
				mesh.normal_tex_ = Texture(*texturedata);
			}

			if (mesh_data[i].specular_tex_data) {
				texturedata = &mesh_data[i].specular_tex_data.value();
				mesh.specular_tex_ = Texture(*texturedata);
			}

			meshes_.emplace_back(std::move(mesh));
		}
	}

	std::optional<std::vector<MeshData>> loadModel(std::string const& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return std::nullopt;
		}

		std::string directory_ = path.substr(0, path.find_last_of('/'));
		std::vector<MeshData> meshes_data_;
		return processNode(scene->mRootNode, scene, directory_, meshes_data_);
	}

	std::optional<TextureData> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory_) {

		TextureData data;
		aiString texture_path;
		if (mat->GetTextureCount(type) == 0) return std::nullopt;

		mat->GetTexture(type, 0, &texture_path);

		std::cout << "tex_map size: " << tex_map.size() << std::endl;
		if (tex_map.find(directory_ + texture_path.C_Str()) == tex_map.end()) {
			data = TextureFromFile(texture_path.C_Str(), directory_);
			//tex_map.insert({ directory_ + texture_path.C_Str(),data });
			tex_map[directory_ + texture_path.C_Str()] = data;
		}
		else {
			data = tex_map[directory_ + texture_path.C_Str()];
			std::cout << tex_map.size();
		}

		return data;
	}

	MeshData processMesh(aiMesh* mesh, const aiScene* scene, std::string directory_) {

		MeshData mesh_data;
		std::vector<Vertex_Data> vertexs;
		std::vector<unsigned int> ids;
		float minX, maxX, minY, maxY, minZ, maxZ;

		minX = maxX = minY = maxY = minZ = maxZ = 0;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex_Data vertex;
			vertex.position.x = mesh->mVertices[i].x;
			if (vertex.position.x < minX) minX = vertex.position.x;
			else if (vertex.position.x > maxX) maxX = vertex.position.x;

			vertex.position.y = mesh->mVertices[i].y;
			if (vertex.position.y < minY) minY = vertex.position.y;
			else if (vertex.position.y > maxY) maxY = vertex.position.y;

			vertex.position.z = mesh->mVertices[i].z;
			if (vertex.position.z < minZ) minZ = vertex.position.z;
			else if (vertex.position.z > maxZ) maxZ = vertex.position.z;

			if (mesh->mNormals) {
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}
			else {
				vertex.normal.x = 0.0f;
				vertex.normal.y = 0.0f;
				vertex.normal.z = 0.0f;
			}

			if (mesh->mTextureCoords != nullptr) {
				vertex.uv.x = mesh->mTextureCoords[0][i].x;
				vertex.uv.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vertex.uv = { 0.0f, 0.0f };
			}
			vertexs.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				ids.push_back(face.mIndices[j]);
			}
		}

		mesh_data.vertexs_ = vertexs;
		mesh_data.ids_ = ids;

		mesh_data.minX = minX;
		mesh_data.minY = minY;
		mesh_data.minZ = minZ;
		mesh_data.maxX = maxX;
		mesh_data.maxY = maxY;
		mesh_data.maxZ = maxZ;

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		mesh_data.diffuse_tex_data = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory_);
		mesh_data.normal_tex_data = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", directory_);
		mesh_data.specular_tex_data = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory_);

		return mesh_data;
	}

	std::vector<MeshData> processNode(aiNode* node, const aiScene* scene, std::string directory_, std::vector<MeshData>& meshes_data_) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes_data_.push_back(processMesh(mesh, scene, directory_));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, directory_, meshes_data_);
		}

		return meshes_data_;
	}
}