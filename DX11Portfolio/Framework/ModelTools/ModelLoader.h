#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

class ModelLoader
{
public:
	void Load(string basePath, string filename);
	void ProcessNode(aiNode* node, const aiScene* scene, Matrix matrix);
	MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);

public:
	string basePath;
	vector<MeshData> meshes;
	
};