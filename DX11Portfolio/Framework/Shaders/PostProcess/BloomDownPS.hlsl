Texture2D g_texture0 : register(t0);        // 입력 텍스처
SamplerState g_sampler : register(s0);      // 샘플러

//Down 패스#1 입력: 960×540 (원본 1920×1080의 1/2)
//Down 패스#2 입력: 480×270
//Down 패스#3 입력: 240×135
// 2px 4px 8px 차이로 읽어내기 위함

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

// 정점 6개인 사각형의 정보 Input
// 가장자리로부터 시작하면 범위를 벗어나게되지만, Sampler의 AddressU = Clamp 이므로 음수가되면 자동으로 0을 선택함.
float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float x = input.texcoord.x;                                                     //현재 픽셀의 x좌표
    float y = input.texcoord.y;                                                     //현재 픽셀의 y좌표
    
    // 2픽셀 차이
    float3 a = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y + 2 * dy)).rgb;    //좌상
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + 2 * dy)).rgb;             //중상
    float3 c = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y + 2 * dy)).rgb;    //우상

    float3 d = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y)).rgb;             //좌중
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;                      //중
    float3 f = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y)).rgb;             //우중

    float3 g = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y - 2 * dy)).rgb;    //좌하
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - 2 * dy)).rgb;             //중하
    float3 i = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y - 2 * dy)).rgb;    //우하

    // 1픽셀 차이
    float3 j = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;            //좌상
    float3 k = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;            //우상
    float3 l = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;            //좌하
    float3 m = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;            //우하

    float3 color = e * 0.125;                                                       //중심 픽셀에는 가중치 0.125 , 1/8
    color += (a + c + g + i) * 0.03125;                                             //먼 대각 지점에는 가중치 0.03125, 1/8
    color += (b + d + f + h) * 0.0625;                                              //먼 상하좌우 지점에는 가중치 0.0625, 1/4
    color += (j + k + l + m) * 0.125;                                               //근처 대각 지점 가중치 0.125, 1/2
                                                                                    //가우시안 근사, 등방성 13개의 픽셀 블러
  
    return float4(color, 1.0);
    //return g_texture0.Sample(g_sampler, input.texcoord);
}