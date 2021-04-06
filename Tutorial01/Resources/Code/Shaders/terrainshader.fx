struct VSIN {
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};
struct HSIN {
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

HSIN VS(VSIN IN) {
	HSIN output = (HSIN)0;
	output.Pos = IN.Pos;
	output.Norm = IN.Norm;
	output.Tex = IN.Tex;
	return output;
}

SamplerState samLinear : register(s0);
Texture2D txDiffuse : register(t0);

struct PSIN {
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex = txDiffuse.Sample(samLinear, IN.Tex);
	return tex;
}