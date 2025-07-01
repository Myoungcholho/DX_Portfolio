#pragma once

#include <DirectXMath.h>           // 또는 Vector3가 정의된 최소 헤더
using namespace DirectX;

struct Material
{
	Vector3 ambient = Vector3(0.1f);
	float shininess = 8.0f;
	Vector3 diffuse = Vector3(0.8f);
	float dummy1;
	Vector3 specular = Vector3(0.5f);
	float dummy2;
};