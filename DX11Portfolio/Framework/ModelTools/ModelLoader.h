#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "Animations/AnimationClip.h"

// aiMesh : 부위별 기하학 데이터 + 본 가중치 정보
// aiNode : 계층 트리 노드, 자신이 참조하는 메시 인덱스 리스트 보유
// aiBone : 메시 내부에서 정점 <-> 본 매핑 정보

class ModelLoader
{
public:
    void ReadAnimation(const aiScene* scene, string filename);

    void Load(string basePath, string filename, bool revertNormals);
    void LoadAnimation(string basePath, string filename);

    const aiNode* FindParent(const aiNode* node);

    void ProcessNode(aiNode* node, const aiScene* scene,DirectX::SimpleMath::Matrix tr);

    PBRMeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);

    string ReadTextureFilename(const aiScene* scene, aiMaterial* material,aiTextureType type);

    void UpdateTangents();

    // 버텍스의 변형에 직접적으로 참여하는 뼈들의 목록을 만듭니다.
    void FindDeformingBones(const aiScene* scene);
    void UpdateBoneIDs(aiNode* node, int* counter);

public:
	string basePath;
	vector<PBRMeshData> meshes;

	AnimationData aniData;

	bool isGLTF = false;

	// Assimp의 LH변환 + (모델에 박힌 루트 변환/음수 스케일) + 수동 축 스왑이 합성되면서
	// 어떤 모델은 최종적으로 det>0 이고
	// 어떤 모델은 det<0 이라 뒤집혀서 토글을 뒀다.
	// det > 0 : 정상 방향 유지 | det < 0 : 뒤집힘 발생
	// det는 그 변환이 공간의 부피를 얼마나 스케일하는지 알려주는 값
	bool revertNormals = false;
};