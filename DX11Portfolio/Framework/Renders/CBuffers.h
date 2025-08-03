#pragma once
#include "Framework.h"

#define MAX_LIGHTS 3

enum class LightType : int
{
    Directional = 0,
    Point = 1,
    Spot = 2
};

struct Light
{
    Vector3 radiance = Vector3(1.0f);                   // 빛의 세기
    float fallOffStart = 0.0f;                          // 감쇠 시작 거리
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);      // 방향
    float fallOffEnd = 10.0f;                           // 감쇠 끝 거리
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);      // 위치
    float spotPower = 100.0f;                           // spot 라이트용
    int type = 0;
    int id = -1;
    float dummy[2];
};

struct WorldConstantBuffer
{
    Matrix World;
};

struct ViewProjectionConstantBuffer
{
    Matrix viewProj;
};

struct MirrorViewProjectionConstantBuffer
{
    Matrix mirrorViewProj;
};

struct TreeConstants
{
    int index;
    int dummy[3];
};

struct WorldInvConstantBuffer
{
    Matrix World;
    Matrix InvTranspose;
    int useHeightMap = 0;
    float heightScale = 0.0f;
    Vector2 dummy;
};

struct NormalVertexCBuffer
{
    float Scale = 0.1f;
    float dummy[3];
};

struct HeightMapConstantBuffer
{
    int useHeightMap = 1;
    float heightScale = 0.0f;
    Vector2 dummy;
};

struct RimParamsConstantBuffer
{
    Vector3 RimColor;
    float RimPower;
    float RimStrength;
    bool useSmoothstep;
    float dummy[2];
};

struct CameraConstantBuffer
{
    Vector3 eyeWorld;         // 카메라 위치 (View space 계산용)
    float mipmapLevel = 0.0f; // 수동 LOD 선택 시
};

struct LegacyMaterialConstantBuffer
{
    LegacyMaterial legacyMaterial;
};

struct LightConstantBuffer
{
    Light lights[MAX_LIGHTS];
    int lightCount;
    Vector3 padding;
};

struct RenderOptionsConstantBuffer
{
    int useTexture = 1;
    int useNormalMap = 1;
    int useAOMap = 1;
    int reverseNormalMapY = 0;
};

struct ObjectCenterConstantBuffer
{
    Vector3 objectCenter;
    float dummy;
};

struct CubeMappingConstantBuffer
{
    int textureToDraw;
    float mipLevel;
    float dummy[2];
};

///////////////////////////////////////////////////////////////////////////////
struct BasicVertexConstantBuffer
{
    Matrix modelWorld;
    Matrix invTranspose;
    Matrix view;
    Matrix projection;
    int useHeightMap = 0;
    float heightScale = 0.0f;
    Vector2 dummy;
};

struct BasicPixelConstantBuffer
{
    Vector3 EyeWorld;
    bool UseTexture;

    Material material;
    Light lights[MAX_LIGHTS];

    int useAlbedoMap = 0;
    int useNormalMap = 0;
    int useAOMap = 0;         // Ambient Occlusion
    int invertNormalMapY = 0; // 16
    int useMetallicMap = 0;
    int useRoughnessMap = 0;
    int useEmissiveMap = 0;
    float expose = 1.0f; // 16
    float gamma = 1.0f;
    Vector3 dummy; // 16
};
///////////////////////////////////////////////////////////////////////////////
struct NormalVertexConstantBuffer
{
    float scale = 0.1f;
    float dummy[3];
};


struct BillboardPointCBuffer
{
    Vector3 EyeWorld;
    float width;
};

///////////////////////////////////////////////////////////////////////////////
struct GlobalConstants
{
    Matrix view;
    Matrix proj;
    Matrix viewProj;
    Vector3 eyeWorld;
    float strengthIBL = 1.0f;
    int textureToDraw = 0;              // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f;            // 환경맵 LodBias
    float lodBias = 2.0f;               // 다른 물체들 LodBias
    float dummy2;

    Light lights[MAX_LIGHTS];
};

struct MaterialConstants {

    Vector3 albedoFactor = Vector3(1.0f); // 12
    float roughnessFactor = 1.0f;
    float metallicFactor = 1.0f;
    Vector3 emissionFactor = Vector3(0.0f);

    // 여러 옵션들에 uint 플래그 하나만 사용할 수 있음 비트 마스킹
    int useAlbedoMap = 0;
    int useNormalMap = 0;
    int useAOMap = 0;
    int invertNormalMapY = 0;
    int useMetallicMap = 0;
    int useRoughnessMap = 0;
    int useEmissiveMap = 0;
    float expose = 1.0f;
    float gamma = 1.0f;
    Vector3 dummy;

};