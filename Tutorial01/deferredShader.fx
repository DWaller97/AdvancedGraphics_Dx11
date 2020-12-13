
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


PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	return float4(0, 0, 0, 1);
}