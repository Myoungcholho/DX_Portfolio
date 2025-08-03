#pragma once

#include <DirectXMath.h>           // 또는 Vector3가 정의된 최소 헤더
using namespace DirectX;

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;


struct Vertex
{
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;
};

struct SkinnedVertex 
{
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;

    float blendWeights[8] = 
    { 
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f 
    };  
    uint8_t boneIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; 
};

struct GrassVertex
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

struct GrassInstance 
{
    Matrix instanceWorld;
    float windStrength;
};