#pragma once

#include <DirectXMath.h>           // 또는 Vector3가 정의된 최소 헤더
using namespace DirectX;

struct FVertexPC
{
    Vector3 position;
    Vector3 color;
};

struct FVertexPNCU
{
    Vector3 position;
    Vector3 normal;
    Vector3 color;
    Vector2 uv;
};

struct FVertex
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};