#include "../VertexStructures.hlsli"

Texture2D g_heightMap : register(t0);
SamplerState g_sampler : register(s0);

cbuffer VertexConstantData : register(b0)
{
    matrix modelWorld;
    matrix invTranspose;
    matrix viewX;
    matrix projectionX;
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

cbuffer CameraConstantBuffer : register(b1)
{
    matrix viewProj;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    // normal
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // tangent
    float4 tangent = float4(input.tangentModel, 1.0f);
    output.tangentWorld = mul(tangent, modelWorld).xyz;
    
    // posworld
    float4 pos = float4(input.posModel, 1.0);
    pos = mul(pos, modelWorld);
    
    if(useHeightMap)
    {
        float height = g_heightMap.SampleLevel(g_sampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
    output.posWorld = pos.xyz;
    
    // pos wvp
    pos = mul(pos, viewProj);
    output.posProj = pos;
    
    // texcoord 
    output.texcoord = input.texcoord;
    
    output.color = float3(0.0f, 0.0f, 0.0f);
    
    return output;
}


