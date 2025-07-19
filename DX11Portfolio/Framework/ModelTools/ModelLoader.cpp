#include "Framework.h"
#include "ModelLoader.h"

#include <DirectXMesh.h>
#include <filesystem>

string GetExtension(const string filename)
{
	// 1. 문자열을 경로 객체로 변경
	// 2. . 이후의 확장자만 가져오고
	// 3. string으로 변경
	string ext(filesystem::path(filename).extension().string());

	// 4. 확장자를 전부 소문자로 변경 (tolower)
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	return ext;
}

void ModelLoader::Load(string basePath, string filename)
{
	// 1. gltf 파일이라면
	if (GetExtension(filename) == ".gltf")
		isGLTF = true;

	this->basePath = basePath;
	Assimp::Importer importer;

	// 2. 모델 파일을 읽어 메모리에 저장
	const aiScene* pScene = importer.ReadFile
	(
		this->basePath + filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
	);

	// 3. 읽었다면 노드 순회하면서 정보 저장
	if (!pScene)
		cout << "failed to read file : " << this->basePath + filename << "\n";
	else
	{
		Matrix matrix;
		ProcessNode(pScene->mRootNode, pScene, matrix);
	}

	UpdateTangents();
}

/// <summary>
/// 3D 모델의 계층 구조를 재귀적으로 순회해 각 노드의 변환 행렬을 누적해 모든 메쉬의 정점 좌표를 모델-월드 기준으로 변환
/// </summary>
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Matrix matrix)
{
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

/// <summary>
/// 메쉬의 정보를 읽어 정점, 인덱스 버퍼와 메쉬가 사용하는 텍스처 경로 설정
/// </summary>
PBRMeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<FVertexPNTT> vertices;
	std::vector<uint32_t> indices;

	// 1. 버텍스 정보 추출
	for (UINT i = 0; i < mesh->mNumVertices; i++) 
	{
		FVertexPNTT vertex;

		// 위치 좌표 복사
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// 노말 복사
		vertex.normalModel.x = mesh->mNormals[i].x;
		if (!isGLTF) // gltf 모델은 y/z축이 뒤바뀌고 y축을 음수로
		{
			vertex.normalModel.y = mesh->mNormals[i].z;
			vertex.normalModel.z = -mesh->mNormals[i].y;
		}
		else 
		{
			vertex.normalModel.y = mesh->mNormals[i].y;
			vertex.normalModel.z = mesh->mNormals[i].z;
		}
		vertex.normalModel.Normalize();

		// 텍스처 좌표가 있다면 복사
		if (mesh->mTextureCoords[0]) 
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	// 2. 인덱스 정보 추출 (삼각형 인덱스)
	for (UINT i = 0; i < mesh->mNumFaces; i++) 
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 3. 결과 메쉬 구조체에 버텍스/인덱스 저장
	PBRMeshData newMesh;
	newMesh.vertices = vertices;
	newMesh.indices = indices;

	// 4. 메테리얼 정보 추출
	if (mesh->mMaterialIndex >= 0) 
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 알베도 읽기
		newMesh.albedoTextureFilename = ReadFilename(material, aiTextureType_BASE_COLOR);

		// ao가 없다면 디퓨즈로 시도
		if (newMesh.aoTextureFilename.empty()) 
		{
			newMesh.aoTextureFilename = ReadFilename(material, aiTextureType_DIFFUSE);
		}
		// ao 가 없다면 LIGHTMAP
		if (newMesh.aoTextureFilename.empty())
		{
			newMesh.aoTextureFilename = ReadFilename(material, aiTextureType_LIGHTMAP);
		}

		newMesh.emissiveTextureFilename = ReadFilename(material, aiTextureType_EMISSIVE);
		newMesh.heightTextureFilename = ReadFilename(material, aiTextureType_HEIGHT);
		newMesh.normalTextureFilename = ReadFilename(material, aiTextureType_NORMALS);
		newMesh.metallicTextureFilename = ReadFilename(material, aiTextureType_METALNESS);
		newMesh.roughnessTextureFilename = ReadFilename(material, aiTextureType_DIFFUSE_ROUGHNESS);
		newMesh.aoTextureFilename = ReadFilename(material, aiTextureType_AMBIENT_OCCLUSION);	
	}

	return newMesh;
}

/// <summary>
/// material에 type 텍스처가 있다면 경로 반환
/// </summary>
string ModelLoader::ReadFilename(aiMaterial* material, aiTextureType type)
{
	// 해당 메테리얼에 지정된 텍스처가 있는지
	if (material->GetTextureCount(type) > 0) 
	{
		aiString filepath;
		// 텍스처 경로 정보 가져오기
		material->GetTexture(type, 0, &filepath);

		// 경로에서 파일 이름만 추출하고 내 경로와 결합
		std::string fullPath = this->basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());

		return fullPath;
	}
	else // 없으면 빈문자열 리턴
	{
		return "";
	}
}

/// <summary>
/// 정점에 탄젠트 정보 추가 등록
/// </summary>
void ModelLoader::UpdateTangents()
{
	// 모든 메쉬들에 대해 루프
	for (auto& m : this->meshes) 
	{
		vector<XMFLOAT3> positions(m.vertices.size());
		vector<XMFLOAT3> normals(m.vertices.size());
		vector<XMFLOAT2> texcoords(m.vertices.size());
		vector<XMFLOAT3> tangents(m.vertices.size());
		vector<XMFLOAT3> bitangents(m.vertices.size());

		// 원본 데이터를 분리해서 복사
		for (size_t i = 0; i < m.vertices.size(); i++) 
		{
			auto& v = m.vertices[i];
			positions[i] = v.position;
			normals[i] = v.normalModel;
			texcoords[i] = v.texcoord;
		}

		// tangent/bitangent 계산을 해준다
		ComputeTangentFrame
		(
			m.indices.data(),			// 삼각형 인덱스
			m.indices.size() / 3,		// 삼각형 개수
			positions.data(),			// 정점 위치 배열
			normals.data(),				// 정점 노멀
			texcoords.data(),			// 정점 UV
			m.vertices.size(),			// 정점 개수
			tangents.data(),			// tangent 저장 위치
			bitangents.data()			// bitangent 저장 위치
		);

		// 계산한 tangent 값을 원본 정점에 할당
		for (size_t i = 0; i < m.vertices.size(); i++) 
		{
			m.vertices[i].tangentModel = tangents[i];
		}
	}
}
