#define NUM_CONTROL_POINTS 4

struct HullShaderInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct HullShaderOutput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

cbuffer CameraConstnatData : register(b0)
{
    float3 eyeWorld;
    float mipmapLevel;
};

cbuffer WorldMatrixConstantBuffer : register(b2)
{
    matrix world;
    matrix invTranspose;
};

cbuffer ObjectCenterConstantBuffer : register(b3)
{
    float3 objectCenter;
    float dummy;
};

struct PatchConstant
{
    float edgeTess[4] : SV_TessFactor;
    float insideTess[2] : SV_InsideTessFactor;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("PatchConstants")]
[maxtessfactor(32.0f)]
HullShaderOutput main(InputPatch<HullShaderInput, NUM_CONTROL_POINTS> patch, uint i : SV_OutputControlPointID)
{
    return patch[i];
}

PatchConstant PatchConstants(InputPatch<HullShaderInput, NUM_CONTROL_POINTS> patch)
{
    PatchConstant pc;

    // 중심 위치 계산
    //float3 avgPos = (patch[0].posModel + patch[1].posModel + patch[2].posModel + patch[3].posModel) / 4.0f;
    //float3 worldPos = mul(float4(avgPos, 1.0f), world).xyz;
    
    float dist = length(eyeWorld - objectCenter);

    float minDist = 3.0f;
    float maxDist = 15.0f;
    float tessLevel = 8.0f * (1.0f - saturate((dist - minDist) / (maxDist - minDist)));
    tessLevel = max(tessLevel, 1.0f);

    pc.edgeTess[0] = tessLevel;
    pc.edgeTess[1] = tessLevel;
    pc.edgeTess[2] = tessLevel;
    pc.edgeTess[3] = tessLevel;
    pc.insideTess[0] = tessLevel;
    pc.insideTess[1] = tessLevel;

    return pc;
}