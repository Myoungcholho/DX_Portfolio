#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

class ModelLoader
{
public:
	void Load(string basePath, string filename);
	void ProcessNode(aiNode* node, const aiScene* scene, Matrix matrix);
	PBRMeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);
	string ReadFilename(aiMaterial* material, aiTextureType type);

public:
	void UpdateTangents(); // ≈∫¡®∆Æ ∫§≈Õ ∞ËªÍ

public:
	string basePath;
	vector<PBRMeshData> meshes;
	bool isGLTF = false;
};