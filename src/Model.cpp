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

	void Model::setUpData(std::vector<MeshData>& mesh_data) {

		// Get max and min in each axis:
		float obj_minX, obj_maxX, obj_minY, obj_maxY, obj_minZ, obj_maxZ;

		obj_minX = obj_maxX = obj_minY = obj_maxY = obj_minZ = obj_maxZ = 0;

		for (int i = 0; i < mesh_data.size(); i++) {

			if (mesh_data[i].minX < obj_minX) obj_minX = mesh_data[i].minX;
			else if (mesh_data[i].maxX > obj_maxX) obj_maxX = mesh_data[i].maxX;

			if (mesh_data[0].minY < obj_minY) obj_minY = mesh_data[i].minY;
			else if (mesh_data[i].maxY > obj_maxY) obj_maxY = mesh_data[i].maxY;

			if (mesh_data[i].minZ < obj_minZ) obj_minZ = mesh_data[i].minZ;
			else if (mesh_data[i].maxZ > obj_maxZ) obj_maxZ = mesh_data[i].maxZ;
		}

		float cx = (obj_minX + obj_maxX) / 2.0f;
		float cy = (obj_minY + obj_maxY) / 2.0f;
		float cz = (obj_minZ + obj_maxZ) / 2.0f;

		float total_max = std::max(obj_maxX - obj_minX, obj_maxY - obj_minY);
		total_max = std::max(total_max, obj_maxZ - obj_minZ);

		for (int i = 0; i < mesh_data.size(); i++) {
			for (int j = 0; j < mesh_data[i].vertexs_.size(); j++) {
				mesh_data[i].vertexs_[j].position.x = (mesh_data[i].vertexs_[j].position.x - cx) / (0.5f * total_max);
				mesh_data[i].vertexs_[j].position.y = (mesh_data[i].vertexs_[j].position.y - cy) / (0.5f * total_max);
				mesh_data[i].vertexs_[j].position.z = (mesh_data[i].vertexs_[j].position.z - cz) / (0.5f * total_max);
			}
		}

		for (int i = 0; i < mesh_data.size(); i++) {
			Mesh mesh = Mesh(mesh_data[i]);

			if (mesh_data[i].diffuse_tex_data) {
				TextureData diffuse_text_data = mesh_data[i].diffuse_tex_data.value();
				mesh.diffuse_tex_ = Texture(diffuse_text_data);
			}

			if (mesh_data[i].normal_tex_data) {
				TextureData normal_text_data = mesh_data[i].normal_tex_data.value();
				mesh.normal_tex_ = Texture(normal_text_data);
			}

			if (mesh_data[i].specular_tex_data) {
				TextureData specular_text_data = mesh_data[i].specular_tex_data.value();
				mesh.specular_tex_ = Texture(specular_text_data);
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

		if (tex_map.find(directory_ + texture_path.C_Str()) == tex_map.end()) {
			data = TextureFromFile(texture_path.C_Str(), directory_);
			tex_map[directory_ + texture_path.C_Str()] = data;
		}
		else {
			data = tex_map[directory_ + texture_path.C_Str()];
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