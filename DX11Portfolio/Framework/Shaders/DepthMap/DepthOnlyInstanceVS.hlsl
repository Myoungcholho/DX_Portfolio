#include "../Common.hlsli"

StructuredBuffer<InstanceData> gInstances : register(t8);

cbuffer SkinnedBatchCB : register(b0)
{
    uint g_MaxBoneCount;
};

float4 main(VertexShaderInput input, uint instanceId : SV_InstanceID) : SV_POSITION
{
#ifdef SKINNED
    float weights[8];
    weights[0] = input.boneWeights0.x;
    weights[1] = input.boneWeights0.y;
    weights[2] = input.boneWeights0.z;
    weights[3] = input.boneWeights0.w;
    weights[4] = input.boneWeights1.x;
    weights[5] = input.boneWeights1.y;
    weights[6] = input.boneWeights1.z;
    weights[7] = input.boneWeights1.w;
    
    uint indices[8];
    indices[0] = input.boneIndices0.x;
    indices[1] = input.boneIndices0.y;
    indices[2] = input.boneIndices0.z;
    indices[3] = input.boneIndices0.w;
    indices[4] = input.boneIndices1.x;
    indices[5] = input.boneIndices1.y;
    indices[6] = input.boneIndices1.z;
    indices[7] = input.boneIndices1.w;

    float3 posModel = 0.0f;
    float3 normalModel = 0.0f;
    float3 tangentModel = 0.0f;

    uint base = instanceId * g_MaxBoneCount;
    
    for(int i = 0; i < 8; ++i)
    {
        uint boneIndex = base + indices[i];
        
        posModel += weights[i] * mul(float4(input.posModel, 1.0f), boneTransforms[boneIndex]).xyz;
        normalModel += weights[i] * mul(input.normalModel, (float3x3) boneTransforms[boneIndex]);
        tangentModel += weights[i] * mul(input.tangentModel, (float3x3) boneTransforms[boneIndex]);
    }

    input.posModel = posModel;
    input.normalModel = normalModel;
    input.tangentModel = tangentModel;
#endif

    InstanceData inst = gInstances[instanceId];
    float4 pos = mul(float4(input.posModel, 1.0f), inst.xform.world);
    return mul(pos, viewProj);
}