struct VSInput_PC
{
    float3 pos : POSITION;
    float3 color : COLOR0;
};

struct PSInput_PC
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

///////////////////////////////////////////////////////////////////////////////

struct VSInput_PNT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct PSInput_PNT
{
    float4 posProj : SV_Position;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 color : COLOR;
};

///////////////////////////////////////////////////////////////////////////////
struct VSInput_PNCT
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct PSInput_PNCT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};
///////////////////////////////////////////////////////////////////////////////
struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float3 normalModel : NORMAL0; // 모델 좌표계의 normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
    float3 color : COLOR; // Normal lines 쉐이더에서 사용
};