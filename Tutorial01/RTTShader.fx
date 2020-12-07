
struct VSIN {
	float4 WorldPos : POSITION;
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PSIN {
	float4 WorldPos : POSITION;
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

SamplerState samLinear : register(s0);
Texture2D txDiffuse : register(t0);

PSIN VS(VSIN IN) {
	PSIN output;
	output.Pos = IN.Pos;
	output.Tex = IN.Tex;
	output.Norm = IN.Norm;
	output.WorldPos = IN.WorldPos;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{

	float4 final = txDiffuse.Sample(samLinear, IN.Tex);
	return final;
}