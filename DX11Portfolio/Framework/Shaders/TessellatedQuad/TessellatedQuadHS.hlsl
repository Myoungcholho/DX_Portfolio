cbuffer ConstantData : register(b0)
{
    float3 eyeWorld;
    float padding;
    Matrix model;
};

cbuffer CameraBuffer : register(b1)
{
    Matrix viewProj;
}

struct VertexOut
{
    float4 pos : POSITION;
};

struct HullOut
{
    float3 pos : POSITION;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

PatchConstOutput MyPatchConstantFunc(InputPatch<VertexOut, 4> patch,
                                     uint patchID : SV_PrimitiveID)
{
    PatchConstOutput pt;
    
    float4 p0 = mul(patch[0].pos, model);
    float4 p1 = mul(patch[1].pos, model);
    float4 p2 = mul(patch[2].pos, model);
    float4 p3 = mul(patch[3].pos, model);
    
    float4 center = (p0 + p1 + p2 + p3) / 4;
    float3 dist = length(center.xyz - eyeWorld);
    float distMin = 0.5;
    float distMax = 5.0;
    float tess = 64.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.edges[3] = tess;
    
    pt.inside[0] = tess;
    pt.inside[1] = tess;
    
    return pt;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(32.0f)]
HullOut main(InputPatch<VertexOut, 4> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HullOut hout;
	
    hout.pos = p[i].pos.xyz;

    return hout;
}
