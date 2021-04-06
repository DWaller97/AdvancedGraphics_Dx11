struct DSIN
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PSIN {

	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float2 Tex : TEXCOORD;
};

struct PatchIN
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

#define NUM_CONTROL_POINTS 3

[domain("tri")]
PSIN main(
	PatchIN input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<DSIN, NUM_CONTROL_POINTS> patch)
{
	PSIN output;
	float4 pos = float4((domain.x * patch[0].Pos + domain.y * patch[1].Pos + domain.z * patch[2].Pos).xyz, 1.0f);
	output.Pos = mul(pos, World);
	output.WorldPos = output.Pos;
	output.Pos = mul(mul(output.Pos, View), Projection);
	output.Tex = patch[0].Tex;

	return output;
}
