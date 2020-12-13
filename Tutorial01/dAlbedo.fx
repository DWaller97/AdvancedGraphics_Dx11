cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}


struct VSIN {
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PSIN {
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D txDiffuse : register(t0);
Texture2D txOcclusion : register(t1);

SamplerState samLinear : register(s0);

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.WorldPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex = txDiffuse.Sample(samLinear, IN.Tex);
	tex.w = txOcclusion.Sample(samLinear, IN.Tex).x;
	return tex;
}