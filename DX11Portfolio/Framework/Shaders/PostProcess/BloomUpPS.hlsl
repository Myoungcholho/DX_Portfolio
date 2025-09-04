Texture2D g_texture0 : register(t0);                        // 블러 적용할 원본
SamplerState g_sampler : register(s0);                      // 샘플러

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength; // Blend 강도, Lerp 강도
    float exposure; // 톤매핑 전 곱할 밝기 스케일
    float gamma; // 감마 인코딩 지수 현재 2.2
    int filterType;
    float option4;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float x = input.texcoord.x;                                     // 현재 U
    float y = input.texcoord.y;                                     // 현재 V
    
    // 이웃 샘플 위쪽 행 (좌,중,우)
    float3 a = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;        
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;

    // 가운대 행 (좌,중,우)
    float3 d = g_texture0.Sample(g_sampler, float2(x - dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + dx, y)).rgb;

    // 아래쪽 행 (좌,중,우)
    float3 g = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;

    // 가중합 : 3x3 가우시안 커널 
    // [1,2,1]
    // [2,4,2]
    // [1,2,1]
    
    float3 color = e * 4.0;                 // 중심 가중치 4
    color += (b + d + f + h) * 2.0;         // 상하좌우 가중치 2
    color += (a + c + g + i);               // 대각선 가중치 1
    color *= 1.0 / 16.0;                    // 4+8+4 = 16으로 정규화
  
    return float4(color, 1.0);
    //return g_texture0.Sample(g_sampler, input.texcoord);
}
