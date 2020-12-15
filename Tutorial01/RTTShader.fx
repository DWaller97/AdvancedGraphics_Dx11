
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

cbuffer SettingsBuffer : register(b1) {
	bool InvertColours;
	bool padding;
	bool padding1;
	bool padding2;
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



SamplerState samLinear : register(s0);
Texture2D txDiffuse : register(t0);

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex = txDiffuse.Sample(samLinear, IN.Tex);
/**********************************************************
	MARKING SCHEME: Simple Screen-Space Effect
	DESCRIPTION: Inverts the colours of the screen
**********************************************************/
	if(InvertColours)
		tex.rgb = 1 - tex.rgb;
	return tex;
}