
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



SamplerState samLinear : register(s0);
Texture2D txDiffuse : register(t0);

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	//output.WorldPos = output.Pos;
	//output.Pos = mul(output.Pos, View);
	//output.Pos = mul(output.Pos, Projection);
	output.Pos = float4(output.Pos.x / output.Pos.w, output.Pos.y / output.Pos.w, output.Pos.z / output.Pos.w, output.Pos.w);
	output.Pos = float4(mul(output.Pos.xyz, 0.5f), output.Pos.w);
	output.Pos = float4(output.Pos.x + 0.5f, output.Pos.y + 0.5f, output.Pos.zw);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{

	float4 final = txDiffuse.Sample(samLinear, IN.Tex);
	return final;
}