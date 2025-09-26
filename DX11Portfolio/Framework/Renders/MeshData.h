#pragma once

#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>

#include "VertexLayout.h"

//struct MeshData
//{
//	vector<GrassVertex> vertices;
//	vector<uint32_t> indices;
//	string textureFilename;
//	string fullFilename;
//};

struct PBRMeshData
{
	vector<Vertex> vertices;
	vector<SkinnedVertex> skinnedVertices;
	vector<uint32_t> indices;
	
	string albedoTextureFilename;		// 기본 색 정의
	string emissiveTextureFilename;		// 이미시브
	string normalTextureFilename;		// 노말 맵

	string heightTextureFilename;		// Height 맵 == DisplcementMap
	string aoTextureFilename;			// 엠비언트 오클루전, 물체 사이 Light 값 계산

	string metallicTextureFilename;		// 메탈릭
	string roughnessTextureFilename;	// 러프니스

	string opacityTextureFilename;
};