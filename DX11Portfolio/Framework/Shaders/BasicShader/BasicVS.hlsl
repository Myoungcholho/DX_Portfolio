#include "../Common.hlsli" // 쉐이더에서도 include 사용 가능

// Vertex Shader에서도 텍스춰 사용
Texture2D g_heightTexture : register(t0);

cbuffer BasicVertexConstantData : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

PixelShaderInput main(VertexShaderInput input)
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
    
    uint indices[8]; // 힌트: 꼭 사용!
    indices[0] = input.boneIndices0.x;
    indices[1] = input.boneIndices0.y;
    indices[2] = input.boneIndices0.z;
    indices[3] = input.boneIndices0.w;
    indices[4] = input.boneIndices1.x;
    indices[5] = input.boneIndices1.y;
    indices[6] = input.boneIndices1.z;
    indices[7] = input.boneIndices1.w;

    float3 posModel = float3(0.0f, 0.0f, 0.0f);
    float3 normalModel = float3(0.0f, 0.0f, 0.0f);
    float3 tangentModel = float3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < 8; ++i)
    {
        posModel += weights[i] * mul(float4(input.posModel, 1.0f), boneTransforms[indices[i]]).xyz;
        normalModel += weights[i] * mul(input.normalModel, (float3x3) boneTransforms[indices[i]]);
        tangentModel += weights[i] * mul(input.tangentModel, (float3x3) boneTransforms[indices[i]]);
    }

    input.posModel = posModel;
    input.normalModel = normalModel;
    input.tangentModel = tangentModel;

#endif
    
    // 뷰 좌표계는 NDC이기 때문에 월드 좌표를 이용해서 조명 계산
    PixelShaderInput output;
    
    // Normal 벡터 먼저 변환 (Height Mapping)
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // Tangent 벡터는 world로 변환
    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    if (useHeightMap)
    {
        // VertexShader에서는 SampleLevel 사용
        // Heightmap은 보통 흑백이라서 마지막에 .r로 float 하나만 사용
        float height = g_heightTexture.SampleLevel(linearWrapSampler, input.texcoord, 0).r;
        //float height = g_heightTexture.Sample(linearWrapSampler, input.texcoord).r;
        // VS는 암시적 미분값(ddx/ddy)가 없어 mip을 고를 수 없음
        
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }

    output.posWorld = pos.xyz; // 월드 위치 따로 저장

    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    return output;
}
