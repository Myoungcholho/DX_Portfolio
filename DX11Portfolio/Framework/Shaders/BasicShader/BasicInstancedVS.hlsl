#include "../Common.hlsli" // 쉐이더에서도 include 사용 가능

Texture2D g_heightTexture : register(t0);
StructuredBuffer<InstanceData> gInstances : register(t8);

cbuffer SkinnedBatchCB : register(b0)
{
    uint g_MaxBoneCount;
};

PixelShaderInputInstance main(VertexShaderInput input, uint instanceId : SV_InstanceID)
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
    
    PixelShaderInputInstance output;
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, inst.xform.worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, inst.xform.world);
    
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, inst.xform.world);
    
    if (inst.xform.useHeightMap != 0)
    {
        float height = g_heightTexture.SampleLevel(linearWrapSampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * inst.xform.heightScale, 0.0);
    }
    
    output.posWorld = pos.xyz;
    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    output.instanced = instanceId;
    
    return output;
}