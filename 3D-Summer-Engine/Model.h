#pragma once
#include <vector>
#include <string>
#include "Mesh.h"

class Model
{
public:
	Model(char* path) {
		loadModel(path);
	}

	void Draw();

private:
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(char* path);
	/*
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName);
	*/
};

