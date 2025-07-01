#include "VertexStructures.hlsli"

TextureCube g_textureCube0 : register(t0);
TextureCube g_diffuseCube : register(t1);
TextureCube g_specularCube : register(t2);
SamplerState g_sampler : register(s0);

float4 main(PSInput_PNT input) : SV_TARGET
{
    // 내부에서 박스를 바라볼 방향이 필요하기 때문에
    // 방향은 3차원이라서 3차원을 전달
    return g_textureCube0.Sample(g_sampler, input.posWorld.xyz);
}