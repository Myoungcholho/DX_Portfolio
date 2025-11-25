#include "Framework.h"
#include "ModelLoader.h"

#include <DirectXMesh.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

#define M_PI 3.14159265358979323846

// 유틸: 쿼터니언 -> Yaw(도)만 간단히 뽑기 (우측손좌표 Y-up 가정)
static float YawDegFromQuaternion(const aiQuaternion& q)
{
	// Yaw(=Y축 회전)만 대략 확인하는 항등식
	// 참고: Assimp 쿼터니언은 (x,y,z,w)
	double siny_cosp = 2.0 * (double(q.w) * q.y + double(q.z) * q.x);
	double cosy_cosp = 1.0 - 2.0 * (double(q.y) * q.y + double(q.x) * q.x);
	double yawRad = std::atan2(siny_cosp, cosy_cosp);
	return float(yawRad * 180.0 / M_PI);
}

// 유틸: 이름이 Hips류인지 판정
static bool IsHipsName(const std::string& n)
{
	// 필요하면 추가: "mixamorig:Hips", "root_Hips", "Armature|Hips" 등
	if (n == "Hips") return true;
	if (n.find("Hips") != std::string::npos) return true;
	return false;
}

/// <summary>
/// 여러 개의 메시를 받아 각각의 vertex normal을 다시 계산하는 함수
/// </summary>
void UpdateNormals(vector<PBRMeshData>& meshes)
{
	for (auto& m : meshes)
	{
		vector<Vector3> normalsTemp(m.vertices.size(), Vector3(0.0f));
		vector<float> weightsTemp(m.vertices.size(), 0.0f);

		// 메시의 인덱스 버퍼를 3개씩 읽어 삼각형 하나씩 순회
		for (int i = 0; i < m.indices.size(); i += 3) 
		{
			int idx0 = m.indices[i];
			int idx1 = m.indices[i + 1];
			int idx2 = m.indices[i + 2];

			Vertex v0 = m.vertices[idx0];
			Vertex v1 = m.vertices[idx1];
			Vertex v2 = m.vertices[idx2];

			// 삼각형의 두 변 벡터를 외적해 법선을 구함
			Vector3 faceNormal = (v1.position - v0.position).Cross(v2.position - v0.position);

			// 구한 법선을 세 정점에 누적, 정점이 속한 삼각형들의 노말을 전부 더함
			normalsTemp[idx0] += faceNormal;
			normalsTemp[idx1] += faceNormal;
			normalsTemp[idx2] += faceNormal;

			// 정점이 겹쳐지면 normal값도 다양하고, 이를 평균내기위해 1씩 누적해둠
			weightsTemp[idx0] += 1.0f;
			weightsTemp[idx1] += 1.0f;
			weightsTemp[idx2] += 1.0f;
		}

		// 모든 정점에 대해 누적된 노말 합계를 나눠 평균을 구함
		for (int i = 0; i < m.vertices.size(); i++) 
		{
			if (weightsTemp[i] > 0.0f) 
			{
				m.vertices[i].normalModel = normalsTemp[i] / weightsTemp[i];
				m.vertices[i].normalModel.Normalize();
			}
		}
	}
}

/// <summary>
/// 파일 경로에서 . 이후의 확장자를 소문자로 변경해 리턴
/// </summary>
string GetExtension(const string filename)
{
	// .이후의 확장자만 가져오고
	string ext(filesystem::path(filename).extension().string());

	// 확장자를 전부 소문자로 변경 (tolower)
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	return ext;
}

///////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Assimp에서 읽어온 애니메이션 정보를 aniData로 옮겨 담는 역할
/// </summary>
void ModelLoader::ReadAnimation(const aiScene *pScene, string filename)
{
    aniData.clips.resize(pScene->mNumAnimations);				// AnimationClip 객체 생성

    for (uint32_t i = 0; i < pScene->mNumAnimations; i++) 
	{
        AnimationClip &clip = aniData.clips[i];				// AnimationClip 데이터 채우기 위해 참조

        const aiAnimation *ani = pScene->mAnimations[i];		// .fbx의 i번째 애니메이션을 꺼냄

        clip.duration = ani->mDuration;							// i번째 애니메이션의 총 길이
        clip.ticksPerSec = ani->mTicksPerSecond;				// i번째 애니메이션의 틱 속도
        clip.keys.resize(aniData.boneNameToId.size());		// 캐릭터 전체 본 개수로 애니메이션 데이터가 있든 없든 bondId로 바로 접근 가능하게 배열 통일
        clip.numChannels = ani->mNumChannels;					// 애니메이션 데이터가 있는 본의 수(디버깅용, 혹은 loop 횟수로도 사용 가능)
		clip.numKeys = static_cast<int>(clip.duration);
		clip.name = filename;

		// 애니메이션이 적용되는 본 개수만큼 반복
        for (uint32_t c = 0; c < ani->mNumChannels; c++) 
		{
            const aiNodeAnim *nodeAnim = ani->mChannels[c];								// 애니메이션에서 변경할 본의 데이터를 가져옴 ex)spine01, leftfoot
			const int boneId = aniData.boneNameToId[nodeAnim->mNodeName.C_Str()];		// 해당 본의 이름을 얻고, 몇번 인덱스인지 얻어옴
            clip.keys[boneId].resize(nodeAnim->mNumPositionKeys);						// 해당 본이 가지는 키 프레임 수만큼 배열 크기를 확보

			// 해당 본이 가지는 키프레임 수만큼 반복
			// 키 프레임 기반 애니메이션과 프레임별 샘플링 애니메이션의 차이가 여기서 나타남,
			// 인덱스를 실행할 프레임 값으로 사용하지 않는다.
			// 
            for (uint32_t k = 0; k < nodeAnim->mNumPositionKeys; k++)
			{
				const aiVector3D pos = nodeAnim->mPositionKeys[k].mValue;				// 포지션 값
                const aiQuaternion rot = nodeAnim->mRotationKeys[k].mValue;				// 회전 값
                const aiVector3D scale = nodeAnim->mScalingKeys[k].mValue;				// 크기 값
				
				AnimationClip::Key &key = clip.keys[boneId][k];							// 해당 본의 k번째 키프레임 데이터를 참조								

                key.pos = {pos.x , pos.y, pos.z };										// k번째 키프레임 위치 데이터를 저장
                key.rot = Quaternion(rot.x, rot.y, rot.z, rot.w);						// k번째 키프레임 회전 데이터 저장

                key.scale = {scale.x, scale.y, scale.z};								// 크기 데이터 저장
				key.timeTicks = nodeAnim->mPositionKeys[k].mTime;						// 키프레임이 진행시간 언제에 해당하는지, 현재 인덱스==mTime이기에 굳이 사용은 안한다.

				// === Hips만 간단 출력 ===
				const std::string chName = nodeAnim->mNodeName.C_Str();
				if (IsHipsName(chName)) 
				{
					std::cout << "frame=" << k
						<< " pos=(" << key.pos.x << ", " << key.pos.y << ", " << key.pos.z << ")"
						<< " rotQ=(" << key.rot.x << ", " << key.rot.y << ", " << key.rot.z << ", " << key.rot.w << ")"
						<< " scale=(" << key.scale.x << ", " << key.scale.y << ", " << key.scale.z << ")"
						<< "\n";
				}
            }
        }
    }
}

void ModelLoader::Load(string basePath, string filename, bool revertNormals)
{
	if (GetExtension(filename) == ".gltf")
	{
		isGLTF = false;
		revertNormals = revertNormals;
	}

	basePath = basePath;

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(
		basePath + filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
	);

	if (pScene)	// 파일 읽었다면
	{
		// 1. 뼈들의 목록 생성 [boneNameToId 셋팅]
		FindDeformingBones(pScene);

		// 2. 트리 구조를 따라 업데이트 순서대로 뼈들의 인덱스 결정 [boneNameToId 셋팅]
		int counter = 0;
		UpdateBoneIDs(pScene->mRootNode, &counter);

		// 3. 업데이트 순서대로 뼈 이름 저장 [boneIdToName 셋팅]
		aniData.boneIdToName.resize(aniData.boneNameToId.size());
		for (pair<const string,int32_t>& i : aniData.boneNameToId)
			aniData.boneIdToName[i.second] = i.first;

		// 4. 각 뼈의 부모 인덱스 저장 준비 [boneParents 셋팅]
		aniData.boneParents.resize(aniData.boneNameToId.size(), -1);
		
		Matrix tr;
		ProcessNode(pScene->mRootNode, pScene, tr);

		// 5. 애니메이션 정보 읽기
		if (pScene->HasAnimations())
			ReadAnimation(pScene, filename);
		
		// 6. tangent 구해서 저장하기
		UpdateTangents();

	}
	else // 파일 못읽었다면
	{
		std::cout << "Failed to read file: " << basePath + filename << std::endl;
		auto errorDescription = importer.GetErrorString();
		std::cout << "Assimp error: " << errorDescription << endl;
	}
}

/// <summary>
/// 모델 정점 데이터는 이미 Load에서 한번 읽어왔으므로 다시 할 필요 없기에
/// 애니메이션 정보만 Load하는 것
/// </summary>
void ModelLoader::LoadAnimation(string basePath, string filename)
{
	basePath = basePath;

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(basePath + filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene && pScene->HasAnimations()) 
	{
		ReadAnimation(pScene,filename);
	}
	else 
	{
		std::cout << "Failed to read animation from file: " << basePath + filename << std::endl;
		auto errorDescription = importer.GetErrorString();
		std::cout << "Assimp error: " << errorDescription << endl;
	}
}

/// <summary>
/// 부모 노드가 본이 아니라면, 그 부모 노드는 계산에 영향받을 노드가 아니라서
/// 즉 나의 상위 본 노드를 찾기 위한 함수
/// </summary>
const aiNode* ModelLoader::FindParent(const aiNode* node)
{
	if (!node)
		return nullptr;

	// 현재 노드 이름이 boneNameToId에 등록되어 있다면, 애니메이션에 실제로 사용되는 본이라면
	// 이 노드가 우리가 찾는 부모 본임.
	if (aniData.boneNameToId.count(node->mName.C_Str()) > 0)
		return node;

	// 만약 상위 노드가 본으로 사용되지 않는다면 본으로 사용되는 노드를 찾기 위해 재귀로 루프
	return FindParent(node->mParent);
}

/// <summary>
/// 루트노드로부터 자식까지 재귀로 순회하면서 만약 노드가 메쉬가 있다면 메쉬 정보를 구성.
/// RootNode
///├─ Armature(스켈레톤 노드, 메쉬 없음)
///│   └─ Hips(Bone, 메쉬 없음)
///│       ├─ Spine(Bone, 메쉬 없음)
///│       │   └─ Chest(Bone, 메쉬 없음)
///│       │       └─ Head(Bone, 메쉬 없음)
///│       └─ LeftArm(Bone, 메쉬 없음)
///│           └─ LeftHand(Bone, 메쉬 없음)
///│       └─ RightArm …
///└─ BodyMeshNode(메쉬 있음, 몸통 메시 연결)
///└─ HairMeshNode(메쉬 있음, 머리카락 메시 연결)
///└─ PantsMeshNode(메쉬 있음, 바지 메시 연결)
/// 이런 느낌이다
/// </summary>
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr)
{
	// 부모가 있고 && boneNameToID에 등록되었으며 && 부모 노드도 역시 bone으로 인식될 수 있다면
	if (node->mParent && aniData.boneNameToId.count(node->mName.C_Str()) && FindParent(node->mParent)) 
	{
		// 부모의 본 번호를 얻어 저장해둠
		const int32_t boneId = aniData.boneNameToId[node->mName.C_Str()];
		aniData.boneParents[boneId] = aniData.boneNameToId[FindParent(node->mParent)->mName.C_Str()];
	}

	// 현재 로컬 행렬 * 부모 행렬로 노드의 WorldMatrix 구축, Transpose는 col -> row
	Matrix m(&node->mTransformation.a1);
	m = m.Transpose() * tr;

	// 노드에 연결된 메시 개수만큼 반복
	for (UINT i = 0; i < node->mNumMeshes; i++) 
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];						// 씬이 가진 메시 배열에서, 노드가 참조하는 메시 인덱스를 꺼내옴
		PBRMeshData newMesh = this->ProcessMesh(mesh, scene);					// aiMesh데이터를 PBRMeshData로 변환

		// 정점들은 전부 자기 메시기준(팔,몸통 등) 로컬 좌표만 가지고 있으므로 부모까지 누적된 좌표계 행렬을 곱해서 월드 좌표계에 맞춤
		for (Vertex& v : newMesh.vertices) 
		{
			v.position = Vector3::Transform(v.position, m);
		}
		meshes.push_back(newMesh);											// 메시하나 구성완료하고 vector에 push
	}

	// 자식 노드 재귀 돌면서 메시 구성 시도, 본인 월드 좌표 행렬도 계산하라고 인자로 넘겨줌
	for (UINT i = 0; i < node->mNumChildren; i++) 
	{
		this->ProcessNode(node->mChildren[i], scene, m);
	}
}

/// <summary>
/// aiMesh를 읽어 PBRMeshData형식으로 변환
/// </summary>
/// <param name="mesh"> Assimp가 파싱한 메시 하나</param>
/// <param name="scene">씬 전체 데이터</param>
/// <returns></returns>
PBRMeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	PBRMeshData newMesh;
	vector<Vertex>& vertices = newMesh.vertices;
	vector<uint32_t>& indices = newMesh.indices;
	vector<SkinnedVertex>& skinnedVertices = newMesh.skinnedVertices;

	// 메쉬가 가지는 정점 수 만큼 반복
	for (UINT i = 0; i < mesh->mNumVertices; i++) 
	{
		Vertex vertex;

		// 1. Position Data 채움
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// 2. Normal Data 채움
		vertex.normalModel.x = mesh->mNormals[i].x;
		if (isGLTF) 
		{
			vertex.normalModel.y = mesh->mNormals[i].z;
			vertex.normalModel.z = -mesh->mNormals[i].y;
		}
		else 
		{
			vertex.normalModel.y = mesh->mNormals[i].y;
			vertex.normalModel.z = mesh->mNormals[i].z;
		}

		if (revertNormals) 
		{
			vertex.normalModel *= -1.0f;
		}

		vertex.normalModel.Normalize();

		// 3. TexCoord Data 채움
		if (mesh->mTextureCoords[0]) 
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		// 4. vector에 push해서 메쉬가 가지는 정점 정보를 구성
		vertices.push_back(vertex);
	}

	// 메시가 가진 면(face) 수 만큼 반복
	for (UINT i = 0; i < mesh->mNumFaces; i++) 
	{
		aiFace face = mesh->mFaces[i];						// i번째 면 정보

		for (UINT j = 0; j < face.mNumIndices; j++)			// 면을 구성하는 정점 인덱스 수
			indices.push_back(face.mIndices[j]);			// 인덱스를 내 인덱스 벡터에 추가
	}

	// 메시가 본을 가지고 있다면
	if (mesh->HasBones()) 
	{
		vector<vector<float>> boneWeights(vertices.size());					// 정점에 각 본이 기여하는 가중치 정보
		vector<vector<uint8_t>> boneIndices(vertices.size());				// 정점에 어떤 본이 영향을 주는지 정보

		// 1회만 해도 되는데 안정성 때문에 계속
		aniData.offsetMatrices.resize(aniData.boneNameToId.size());		// 본별 오프셋 행렬(바인드포즈 → 로컬 변환)
		//aniData.boneTransforms.resize(aniData.boneNameToId.size());		// 런타임에 갱신되는 본 최종 변환

		int count = 0;
		// 메시에서 사용되는 본의 개수
		for (uint32_t i = 0; i < mesh->mNumBones; i++) 
		{
			// 본을 가져오고
			const aiBone* bone = mesh->mBones[i];

			// 본의 이름으로 매핑된 인덱스를 가져옴
			const uint32_t boneId = aniData.boneNameToId[bone->mName.C_Str()];

			// 모델 공간 좌표 -> 본 로컬 좌표로 바꿔주는 행렬 저장
			// 다른 메시에서 덮어쓰기가 발생할 수 있지만 같은 뼈에 속하므로 실제 값은 동일해 문제 없다.
			// 전 메시에서 일관된 값이므로
			aniData.offsetMatrices[boneId] = Matrix((float*)&bone->mOffsetMatrix).Transpose();
			//Matrix scale = Matrix::CreateScale(0.1f);
			//aniData.offsetMatrices[boneId] = aniData.offsetMatrices[boneId] * scale;


			// 본이 영향을 주는 정점 개수만큼 반복
			// 메시 -> 본 -> 영향주는 정점으로 온 것
			for (uint32_t j = 0; j < bone->mNumWeights; j++)
			{
				aiVertexWeight weight = bone->mWeights[j];							// 정점번호와 가중치 정보가 있음
				assert(weight.mVertexId < boneIndices.size());

				boneIndices[weight.mVertexId].push_back(boneId);					// [정점번호] = 영향받을 본 번호
				boneWeights[weight.mVertexId].push_back(weight.mWeight);			// [정점번호] = 영향받을 가중치 기록
			}
		}

		// SkinnedMesh Data 구성 시작
		skinnedVertices.resize(vertices.size());
		for (int i = 0; i < vertices.size(); i++) 
		{
			// 원래 Vertex의 기본 속성 복사
			skinnedVertices[i].position = vertices[i].position;
			skinnedVertices[i].normalModel = vertices[i].normalModel;
			skinnedVertices[i].texcoord = vertices[i].texcoord;

			// 이 정점이 영향을 받는 모든 본 정보 기록
			for (int j = 0; j < boneWeights[i].size(); j++) 
			{
				skinnedVertices[i].blendWeights[j] = boneWeights[i][j];			// 정점 i가 bone j에 받는 가중치
				skinnedVertices[i].boneIndices[j] = boneIndices[i][j];			// 정점 i에 영향을 주는 bone의 인덱스(ID)
			}
		}

		// 메시가 가지는 텍스처 정보 이름 얻어와 저장
		if (mesh->mMaterialIndex >= 0) 
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			newMesh.albedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_BASE_COLOR);
			if (newMesh.albedoTextureFilename.empty()) {
				newMesh.albedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_DIFFUSE);
			}
			newMesh.emissiveTextureFilename = ReadTextureFilename(scene, material, aiTextureType_EMISSIVE);
			newMesh.heightTextureFilename = ReadTextureFilename(scene, material, aiTextureType_HEIGHT);
			newMesh.normalTextureFilename = ReadTextureFilename(scene, material, aiTextureType_NORMALS);
			newMesh.metallicTextureFilename = ReadTextureFilename(scene, material, aiTextureType_METALNESS);
			newMesh.roughnessTextureFilename = ReadTextureFilename(scene, material, aiTextureType_DIFFUSE_ROUGHNESS);
			newMesh.aoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_AMBIENT_OCCLUSION);
			if (newMesh.aoTextureFilename.empty()) {
				newMesh.aoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_LIGHTMAP);
			}
			newMesh.opacityTextureFilename = ReadTextureFilename(scene, material, aiTextureType_OPACITY);

			if (!newMesh.opacityTextureFilename.empty()) {
				cout << newMesh.albedoTextureFilename << endl;
				cout << "Opacity " << newMesh.opacityTextureFilename << endl;
			}
		}

		return newMesh;
	}
}

/// <summary>
/// aiScene은 모든 메시의 메테리얼 정보를 배열로 가지고 있고 메쉬는 본인이 사용되는 메테리얼의 인덱스 번호만 가지고 있다.
/// 따라서 그 인덱스 번호에 접근해 aiMaterial*를 얻고 이 정보에 type이 있는지 확인하고 있다면 경로를 저장한다.
/// aiScene
///├─ mMeshes[]
///│    ├─ mesh[0].mMaterialIndex = 0  --->mMaterials[0]
///│    ├─ mesh[1].mMaterialIndex = 1  --->mMaterials[1]
///│    └─ mesh[2].mMaterialIndex = 0  --->mMaterials[0](같은 재질 공유)
///│
///└─ mMaterials[]
///├─[0] Material(diffuse = skin.png, normal = skin_n.png …)
///├─[1] Material(diffuse = cloth.png, normal = cloth_n.png …)
///└─ ... 
/// 
/// 이런 느낌이다.
/// </summary>
string ModelLoader::ReadTextureFilename(const aiScene* scene, aiMaterial* material, aiTextureType type)
{
	// 메쉬의 메테리얼 인덱스를 얻고 씬이 관리하는 메테리얼들의 인덱스의 정보가 있다면
	// 그리고 그 Type에 값이 저장되어 있다면
	if (material->GetTextureCount(type) > 0) 
	{
		// 파일 문자열을 얻어옴
		aiString filepath;
		material->GetTexture(type, 0, &filepath);

		// 풀 파일 경로를 만들고
		string fullPath = basePath + string(filesystem::path(filepath.C_Str()).filename().string());

		// 1. 실제로 파일이 존재하는지 확인
		if (!filesystem::exists(fullPath)) 
		{
			// 2. 파일이 없을 경우 혹시 fbx 자체에 Embedded인지 확인
			const aiTexture* texture = scene->GetEmbeddedTexture(filepath.C_Str());
			if (texture) 
			{
				// 3. Embedded texture가 존재하고 png일 경우 저장
				// 즉 임베디드된 png파일을 읽어서 파일로 만듬
				if (string(texture->achFormatHint).find("png") != string::npos) 
				{
					ofstream fs(fullPath.c_str(), ios::binary | ios::out);
					fs.write((char*)texture->pcData, texture->mWidth);
					fs.close();
				}
			}
			else 
			{
				// 파일도 없고 임베디드도 없는 경우
				cout << fullPath << " doesn't exists. Return empty filename." << endl;
			}
		}
		else 
		{
			// 파일이 존재하므로 경로 반환
			return fullPath;
		}

		// 파일이 존재하므로 경로 반환
		return fullPath;
	}
	else 
	{
		// 메테리얼이 해당 파일을 가지고 있지 않으므로 빈 문자 반환
		return "";
	}
}

void ModelLoader::UpdateTangents()
{
	// 메시 전부 순회
	for (PBRMeshData& m : this->meshes)
	{
		vector<XMFLOAT3> positions(m.vertices.size());
		vector<XMFLOAT3> normals(m.vertices.size());
		vector<XMFLOAT2> texcoords(m.vertices.size());
		vector<XMFLOAT3> tangents(m.vertices.size());
		vector<XMFLOAT3> bitangents(m.vertices.size());

		for (size_t i = 0; i < m.vertices.size(); i++) 
		{
			auto& v = m.vertices[i];
			positions[i] = v.position;
			normals[i] = v.normalModel;
			texcoords[i] = v.texcoord;
		}

		// 삼각형/정점/UV 정보를 기반으로 각 정점의 Tangent, Bitangent를 계산
		ComputeTangentFrame(m.indices.data(), m.indices.size() / 3, positions.data(), normals.data(), texcoords.data(), m.vertices.size(), tangents.data(), bitangents.data());

		for (size_t i = 0; i < m.vertices.size(); i++) 
		{
			m.vertices[i].tangentModel = tangents[i];
		}

		if (m.skinnedVertices.size() > 0) 
		{
			for (size_t i = 0; i < m.skinnedVertices.size(); i++) 
			{
				m.skinnedVertices[i].tangentModel = tangents[i];
			}
		}
	}
}

/// <summary>
/// 의도치 않은 삽입을 막고, 스킨에 실제로 쓰는 뼈 집합을 먼저 확정해서
/// 이후 인덱스 기반 자료구조와 매핑을 일관·안전하게 만들려고 미리 등록
/// </summary>
void ModelLoader::FindDeformingBones(const aiScene* scene)
{
	// 씬의 모든 메쉬를 훑는다
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];						// mesh정보를 얻고

		// 메쉬가 본을 가지고 있다면
		if (mesh->HasBones())
		{
			// 메쉬가 가지는 본의 개수만큼 반복
			for (uint32_t i = 0; i < mesh->mNumBones; i++) 
			{
				// 발견한 본을 map에 등록, 초기값은 -1로 셋팅
				const aiBone* bone = mesh->mBones[i];
				aniData.boneNameToId[bone->mName.C_Str()] = -1;
			}
		}
	}
}

/// <summary>
/// 본에 ID를 부여 아래와 같은 식
/// root 
/// - hip (0)
/// --Spine (1)
/// ---Chest (2)
/// ---- Head (3)
/// ---LeftShoulder (4)
/// ----leftArm (5)
/// -----leftHand (6)
/// -- RightShoulder (7)
/// ---RightArm (8)
/// ----RightHand (9)
/// </summary>
void ModelLoader::UpdateBoneIDs(aiNode* node, int* counter)
{
	if (node)
	{
		// node의 mName이 존재하면 이 노드는 변형에 쓰이는 뼈로 -1로 초기화된 값을 변경함
		if (aniData.boneNameToId.count(node->mName.C_Str()))
		{
			aniData.boneNameToId[node->mName.C_Str()] = *counter;
			*counter += 1;
		}

		// 현재 노드의 모든 자식 노드를 재귀 호출해서 같은 과정을 반복
		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			UpdateBoneIDs(node->mChildren[i], counter);
		}
	}
}
