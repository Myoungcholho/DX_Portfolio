#include "VertexStructures.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PSInput_PNCT input) : SV_TARGET
{
    return g_texture0.Sample(g_sampler, input.uv);
	//return float4(1.0f, 0.0f, 1.0f, 1.0f);
}