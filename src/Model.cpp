#include "mew/Model.hpp"
#include <iostream>
#include <GL/glew.h>
#include <gl/GL.h>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <unordered_map>

namespace MEW {
	Model::Model()
	{ }
	Model::Model(const std::vector<MeshData>& meshData)
	{
		loadMeshes(meshData);
	}

	void Model::loadMeshes(const std::vector<MeshData>& meshData)
	{
		for (int i = 0; i < meshData.size(); i++) {
			Mesh mesh = Mesh(meshData[i]);
			if (meshData[i].tex_data) {
				std::vector<TextureData> diffuse_text_data = meshData[i].tex_data.value();
				size_t td_size = diffuse_text_data.size();
				for (int j = 0; j < td_size; j++) {
					mesh.textures_.emplace_back(Texture(diffuse_text_data.at(j)));
				}
			}

			meshes.emplace_back(std::move(mesh));
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
		processNode(scene->mRootNode, scene, directory_, meshes_data_);
		return meshes_data_;
	}

	std::optional<std::vector<TextureData>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory_) {

		std::vector<TextureData> textures_data;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

			aiString texture_path;
			mat->GetTexture(type, i, &texture_path);

			TextureData data = TextureFromFile(texture_path.C_Str(), directory_);
			//texture.path = texture_path.C_Str();
			textures_data.push_back(data);
		}
		if (textures_data.empty()) return std::nullopt;
		else return textures_data;
	}

	MeshData processMesh(aiMesh* mesh, const aiScene* scene, std::string directory_) {

		MeshData mesh_data;
		std::vector<VertexData> vertexs;
		std::vector<unsigned int> ids;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			VertexData vertex;
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			if (mesh->mNormals) {
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}
			else {
				vertex.Normal.x = 0.0f;
				vertex.Normal.y = 0.0f;
				vertex.Normal.z = 0.0f;
			}

			if (mesh->mTextureCoords != nullptr) {
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vertex.TexCoords = { 0.0f, 0.0f };
			}
			vertexs.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				ids.push_back(face.mIndices[j]);
			}
		}

		mesh_data.vertices_ = vertexs;
		mesh_data.ids_ = ids;

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		mesh_data.tex_data = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory_);

		return mesh_data;
	}

	void processNode(aiNode* node, const aiScene* scene, std::string directory_, std::vector<MeshData>& meshes_data_) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes_data_.push_back(processMesh(mesh, scene, directory_));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, directory_, meshes_data_);
		}

	}
} 