#include "Framework.h"
#include "ModelLoader.h"

#include <filesystem>

void ModelLoader::Load(string basePath, string filename)
{
	this->basePath = basePath;

	Assimp::Importer importer;

	// 1. 모델 파일을 읽어 메모리에 저장
	const aiScene* pScene = importer.ReadFile
	(
		this->basePath + filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
	);

	
	Matrix matrix;
	ProcessNode(pScene->mRootNode, pScene, matrix);

	
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Matrix matrix)
{
	//std::cout << "[Node] " << node->mName.C_Str() << std::endl;

	Matrix m;
	// 요소에 접근하기 위한 4x4매트릭스 첫번째 주소
	ai_real* temp = &node->mTransformation.a1;
	// 지역 변수 m에 복사
	float* mTemp = &m._11;
	for (int t = 0; t < 16; ++t)
	{
		mTemp[t] = float(temp[t]);
	}
	// row major -> col major로 전치
	m = m.Transpose() * matrix;

	// 하나의 노드에 여러 매쉬가 있을 수 있어 전부 처리
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto newMesh = this->ProcessMesh(mesh, scene);

		for (auto& v : newMesh.vertices)
		{
			v.position = DirectX::SimpleMath::Vector3::Transform(v.position, m);
		}

		meshes.push_back(newMesh);
	}

	// 현재 노드의 모든 자식 노드에 대해 재귀적 처리, 자식에게는 현재까지 누적된 트렌스폼 전달
	for (UINT i = 0; i < node->mNumChildren; i++)
	{

		this->ProcessNode(node->mChildren[i], scene, m);
	}
}

// aiMesh 정보를 MeshData로 변환
MeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<FVertex> vertices;
	vector<uint32_t> indices;

	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		FVertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;
		vertex.normal.Normalize();

		if (mesh->mTextureCoords[0])
		{
			vertex.uv.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.uv.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}
	
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	// 내보낼 MeshData 구조체 정보 셋팅
	MeshData newMesh;
	newMesh.vertices = vertices;
	newMesh.indices = indices;
	
	// 메테리얼이 사용하는 텍스처의 파일명을 얻어옴
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString filepath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

			string fullPath = this->basePath + 
				string(std::filesystem::path(filepath.C_Str()).filename().string());

			newMesh.textureFilename = fullPath;

			//string TestPath = this->basePath + string(filepath.C_Str());
			//newMesh.fullFilename = TestPath;
		}
	}

	return newMesh;
}
