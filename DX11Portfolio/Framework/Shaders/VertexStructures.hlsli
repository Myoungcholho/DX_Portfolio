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
