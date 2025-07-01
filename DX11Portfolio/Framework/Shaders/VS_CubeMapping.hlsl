#include "VertexStructures.hlsli"

cbuffer ModelBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
};

cbuffer CameraBuffer : register(b2)
{
    float4x4 view;
    float4x4 projection;
};

PSInput_PNT main(VSInput_PNT input)
{
    PSInput_PNT output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);

    output.posWorld = pos.xyz;
    
    float4 normal = float4(input.normal, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    pos = mul(pos, view);
    
    pos = mul(pos, projection);
    output.posProj = pos;

    output.texcoord = input.texcoord;
    output.color = float3(1.0, 1.0, 0.0);

    return output;
}
