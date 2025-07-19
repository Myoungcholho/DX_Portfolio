#include "../VertexStructures.hlsli"

cbuffer ModelBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
};

// »ç¿ë X
cbuffer CameraBufferOrigin : register(b1)
{
    float4x4 view;
    float4x4 projection;
}

cbuffer CameraBuffer : register(b2)
{
    float4x4 TranslationView;
    float4x4 Translationprojection;
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
    pos = mul(pos, TranslationView);
    pos = mul(pos, Translationprojection);
    output.posProj = pos;

    output.posModel = input.posModel;
    
    //output.posModel = input.posModel;
    //output.posProj = mul(float4(input.posModel, 1.0f), view);
    //output.posProj = mul(output.posProj, projection);

    return output;
}
