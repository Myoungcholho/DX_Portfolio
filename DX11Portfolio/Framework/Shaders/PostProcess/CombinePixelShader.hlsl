Texture2D g_texture0 : register(t0);                // 씬 컬러 HDR
Texture2D g_texture1 : register(t1);                // Bloom 결과 HDR
SamplerState g_sampler : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;                                 // Blend 강도, Lerp 강도
    float exposure;                                 // 톤매핑 전 곱할 밝기 스케일
    float gamma;                                    // 감마 인코딩 지수 현재 2.2
    int filterType;
    float option4;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// 이건 빛번점이 없음
// 단순히 원본 + 흐린 이미지를 HDR->LDR로 사용할 수 있게 바꾸고, 감마 보정만 적용한 것
float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;      // 감마 역수(1/2.2)

    color = clamp(exposure * color, 0., 1.);        // 노출 적용 후 0~1로 클램프, 동적범위 압축이 없어 HDR의 1이상값은 다 잘려나간다
    color = pow(color, invGamma);                   // 감마 적용
    return color;                                   // sRGB로 인코딩된 LDR
}

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);            // 음수값 클램프
    // Hejl/Burgess-Dawson 계열 '필믹' 톤매핑 커브 (유리식: 다항/다항)
    //  - 어두운 영역(Toe)을 살짝 들어 올려 블랙 크러시 완화
    //  - 중간톤은 비교적 선형에 가깝게 유지
    //  - 밝은 영역(Shoulder)은 부드럽게 눌러 0~1 안으로 말아 넣음
    //  - x→∞에서 분자/분모의 x^2 계수가 동일(6.2) → 출력이 1에 점근(부드러운 롤오프)
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    //color = pow(color, float3(1.0, 1.0, 1.0) / gamma);
    return color;                                   // LDR로 압축된 값, 감마는 미포함
}

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15; // 토(Toe)와 미드톤 시작을 잡는 항의 계수
    float B = 0.50; // 미드톤 경사(slope) 관련
    float C = 0.10; // 토(Toe) 형태 조정
    float D = 0.20; // 숄더(Shoulder) 위치/완만함에 기여
    float E = 0.02; // 바닥 오프셋(블랙 리프트) 역할
    float F = 0.30; // 숄더 강도/포화 제어
    float W = 11.2; // 화이트포인트(이 밝기를 1.0으로 보이게 맞춤)
    
    // 1) 노출: HDR 선형값을 전체적으로 키우거나 줄임
    color *= exposure;
    
    // 2) 필믹 톤매핑 곡선(채널별 적용): 
    //    f(x) = ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E/F
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    
    // 3) 화이트포인트 정규화: f(W)를 1.0에 매핑해 전체 스케일을 맞춤
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    
    // 4) 감마 인코딩: 선형 LDR → sRGB. (여기서는 함수 내부에 포함)
    color = pow(color, float3(1.0, 1.0, 1.0) / gamma);
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / gamma;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
    return color;
}

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float3 color0 = g_texture0.Sample(g_sampler, input.texcoord).rgb;
    float3 color1 = g_texture1.Sample(g_sampler, input.texcoord).rgb;
    
    float3 combined = (1.0 - strength) * color0 + strength * color1;
    
    switch (filterType)
    {
        case 0:
            combined = LinearToneMapping(combined);
            break;
        case 1:
            combined = FilmicToneMapping(combined);
            break;
        case 2:
            combined = Uncharted2ToneMapping(combined);
            break;
        case 4:
            combined = lumaBasedReinhardToneMapping(combined);
            break;
    }
    
    return float4(combined, 1.0f);
}