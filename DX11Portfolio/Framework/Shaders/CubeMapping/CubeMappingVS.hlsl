#include "../VertexStructures.hlsli"

cbuffer ModelBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
};

// »ç¿ë X
cbuffer CameraBufferOrigin : register(b1)
{
    float4x4 viewProj;
}

cbuffer CameraBuffer : register(b2)
{
    float4x4 TranslationViewProj;
};

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_Position;
    float3 posModel : POSITION;
};

CubeMappingPixelShaderInput main(VertexShaderInput input)
{
    CubeMappingPixelShaderInput output;
    float4 pos = float4(input.posModel, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, TranslationViewProj);
    output.posProj = pos;

    output.posModel = input.posModel;
    
    //output.posModel = input.posModel;
    //output.posProj = mul(float4(input.posModel, 1.0f), view);
    //output.posProj = mul(output.posProj, projection);

    return output;
}
