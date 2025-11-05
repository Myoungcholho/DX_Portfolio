#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

// 쉐이더에서 include할 내용들은 .hlsli 파일에 작성
// Properties -> Item Type: Does not participate in build으로 설정

#define MAX_LIGHTS 2 // 쉐이더에서도 #define 사용 가능
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

// 샘플러들을 모든 쉐이더에서 공통으로 사용
SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);
SamplerState shadowPointSampler : register(s2);
SamplerComparisonState shadowCompareSampler : register(s3);
SamplerState anisoWrapSampler : register(s4);
SamplerState anisoClampSampler : register(s5);

// 공용 텍스춰들 t10 부터 시작
TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);

Texture2D shadowMaps[MAX_LIGHTS] : register(t15);

struct Light
{
    float3 radiance;        // 빛의 색과 세기
    float fallOffStart;     // Point/Spot일때 사용
    float3 direction;       // Directional/Spot용
    float fallOffEnd;       // Point/Spot일때 사용
    float3 position;        // Point/Spot일때 사용
    float spotPower;        // Spot일때 사용
    uint type;               // 0 : Directional, 1 : Point, 2 : Spot
    
    int id;
    bool enabled;
    float radius;
    
    matrix viewProj;
    matrix invProj;
};

struct InstanceTransform
{
    matrix world;
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

struct InstanceMaterial
{
    float3 albedoFactor; // baseColor
    float roughnessFactor;
    float metallicFactor;
    float3 emissionFactor;

    int useAlbedoMap;
    int useNormalMap;
    int useAOMap; // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    float exposure;
    float gamma;
    float3 dummy0;
};

struct InstanceData
{
    InstanceTransform xform;
    InstanceMaterial mat;
};

// 공용 Constants
cbuffer GlobalConstants : register(b1)
{
    matrix view;
    matrix proj;
    matrix invProj; // 역프로젝션행렬
    matrix viewProj;
    matrix invViewProj; // Proj -> World
    float3 eyeWorld;
    float strengthIBL;

    int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 2.0f; // 다른 물체들 LodBias
    float dummy2;
    
    Light lights[MAX_LIGHTS];
};

#ifdef SKINNED
StructuredBuffer<float4x4> boneTransforms : register(t9);
#endif

struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float3 normalModel : NORMAL0; // 모델 좌표계의 normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
    
#ifdef SKINNED
    float4 boneWeights0 : BLENDWEIGHT0;
    float4 boneWeights1 : BLENDWEIGHT1;
    uint4 boneIndices0 : BLENDINDICES0;
    uint4 boneIndices1 : BLENDINDICES1;
#endif
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
};

struct PixelShaderInputInstance
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
    uint instanced : TEXCOORD1;
};

#endif // __COMMON_HLSLI__