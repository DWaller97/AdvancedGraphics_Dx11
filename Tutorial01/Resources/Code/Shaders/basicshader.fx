cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.WorldPos = output.Pos;
	output.Pos = mul(mul(output.Pos, View), Projection);

	output.Tex = input.Tex;
	return output;
}


float4 PS(PS_INPUT IN) : SV_TARGET
{
	float4 finalColor = float4(IN.WorldPos.x / 256, IN.WorldPos.y / 256, IN.WorldPos.z / 256, 1);

	return finalColor;
}