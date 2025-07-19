#include "../VertexStructures.hlsli"
#include "../Common.hlsli"

TextureCube specularIBLTex : register(t0);
TextureCube irradianceIBLTex : register(t1);
Texture2D brdfTex : register(t2);
Texture2D albedoTex : register(t3);
Texture2D normalTex : register(t4);
Texture2D aoTex : register(t5);
Texture2D metallicRoughnessTex : register(t6);
Texture2D emissiveTex : register(t7);

SamplerState linearSampler : register(s0);
SamplerState clampSampler : register(s1);

static const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0

cbuffer BasicPixelConstData : register(b0)
{
    float3 eyeWorld;
    float mipmapLevel;
    Material material;
    Light light_x[MAX_LIGHTS];
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap; // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    float exposure;
    float gamma;
    float3 dummy;
};

cbuffer LightConstantBuffer : register(b2)
{
    Light light[MAX_LIGHTS];
    int lightCount;
    float3 padding;
};

// 슐리크-프레넬
// 반사 계수를 구하는 것
// 수직이면 적은 반사 평행이라면 반사가 강해짐
// 둘다 pow를 쓰는데 왜 빠르냐면
// pow(x,y) pow(2,y) 는 내부적으로 다르기 때문임 2는 쉬프트 사용가능
float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH); // 빠른 근사치 공식
    //return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); // Schlick 기본 공식
}

struct PixelShaderOutput
{
    float4 pixelColor : SV_TARGET0;
};

// 노말맵을 사용하는 경우에 텍스처의 탄젠트 공간을 world Normal로 변환하기 위해 사용
float3 GetNormal(PixelShaderInput input)
{
    float3 normalWorld = input.normalWorld;
    
    if(useNormalMap)
    {
        float3 normal = normalTex.SampleLevel(linearSampler, input.texcoord, 0).rgb;
        normal.y = invertNormalMapY ? -normal.y : normal.y;
        
        float3 N = normalWorld;
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normalWorld, TBN));
    }
    
    return normalWorld;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye, float metallic)
{
    // 1. 금속성과 비금속에 따라 반사율(F0) 결정 (Fdielectric: 비금속 기준값, albedo: 금속색)
    float3 F0 = lerp(Fdielectric, albedo, metallic);

    // 2. 프레넬 반사율 계산 (시점과 노멀 각도에 따라 반사율 가중치 가짐)
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));

    // 3. 확산 반사 비율(에너지 보존): 
    // 금속이면 kd는 0 (금속은 확산반사가 없음)
    // 비금속이면 kd = 1-F (reflection 제외한 나머지 에너지)
    float3 kd = lerp(1.0 - F, 0.0, metallic);

    // 4. 환경광/IBL용 정보(irradiance map에서 노멀 방향으로 환경광 샘플링)
    float3 irradiance = irradianceIBLTex.Sample(linearSampler, normalWorld).rgb;

    // 5. 최종 디퓨즈 IBL 결과: 확산 에너지 * 표면색 * 환경간접광
    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(clampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearSampler, reflect(-pixelToEye, normalWorld), 3 + roughness * 5.0f).rgb;
    
    const float3 Fdielectric = 0.04;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    
    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao, float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    
    return (diffuseIBL + specularIBL) * ao;
}

// BRDF에서 D(h) 부분
float NdfGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaSq / (3.141592 * denom * denom);
}

// 표면이 거칠수록 1에 가까워짐
// k값이 크면(러프니스가) 감쇠 효과가 더 강하게 나타나 작은 값 return
float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float SchlickGGX(float NdotI, float NdotO, float roughness)
{
		// 거칠기를 1.0만큼 보정해 사람 눈에 자연스럽게
    float r = roughness + 1.0;
    // 거칠수록 감쇠 값 증가함
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

PixelShaderOutput main(PixelShaderInput input)
{
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input);
    
    float3 albedo = useAlbedoMap ? albedoTex.Sample(linearSampler, input.texcoord).rgb : material.albedo;
    float ao = useAOMap ? aoTex.SampleLevel(linearSampler, input.texcoord,0.0).r : 1.0;
    float metallic = useMetallicMap ? metallicRoughnessTex.Sample(linearSampler, input.texcoord).b: material.metallic;
    float roughness = useRoughnessMap ? metallicRoughnessTex.Sample(linearSampler, input.texcoord).g : material.roughness;
    float3 emission = useEmissiveMap ? emissiveTex.Sample(linearSampler, input.texcoord).rgb : float3(0, 0, 0);
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness);
    float3 directLighting = float3(0, 0, 0);
    
    // 포인트 라이트만 먼저 구현
    [unroll]
    for (int i = 0; i < lightCount; ++i)
    {
        
        
            float3 lightVec = light[i].position - input.posWorld;
            float3 halfway = normalize(pixelToEye + lightVec);
        
            float NdotI = max(0.0, dot(normalWorld, lightVec));
            float NdotH = max(0.0, dot(normalWorld, halfway));
            float NdotO = max(0.0, dot(normalWorld, pixelToEye));
        
            const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
            float3 F0 = lerp(Fdielectric, albedo, metallic);
            float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
            float3 diffuseBRDF = kd * albedo;

            float D = NdfGGX(NdotH, roughness);
            float3 G = SchlickGGX(NdotI, NdotO, roughness);
            float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);

            float3 radiance = light[i].radiance * saturate((light[i].fallOffEnd - length(lightVec)) / (light[i].fallOffEnd - light[i].fallOffStart));

            directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
        
        //directLighting += (specularBRDF) * radiance * NdotI;
    }
    
    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting + emission, 1.0);
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}