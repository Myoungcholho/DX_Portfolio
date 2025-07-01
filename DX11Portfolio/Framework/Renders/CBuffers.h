#pragma once
#include "Framework.h"

#define MAX_LIGHTS 3

struct Light
{
    Vector3 strength = Vector3(1.0f);
    float fallOffStart = 0.0f;
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    float fallOffEnd = 10.0f;
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
    float spotPower = 1.0f;
};

struct WorldCBuffer
{
    Matrix World;
};

struct TreeConstants
{
    int index;
    int dummy[3];
};

struct ModelCBuffer
{
    Matrix Model;
    Matrix InvTranspose;
};

struct NormalVertexCBuffer
{
    float Scale = 0.1f;
    float dummy[3];
};


struct BasicPixelConstantBuffer
{
    Vector3 EyeWorld;
    bool UseTexture;
    Material material;
    Light lights[MAX_LIGHTS];
};

struct RimParamsCBuffer
{
    Vector3 RimColor;
    float RimPower;
    float RimStrength;
    bool useSmoothstep;
    float dummy[2];
};

struct BillboardPointCBuffer
{
    Vector3 EyeWorld;
    float width;
};