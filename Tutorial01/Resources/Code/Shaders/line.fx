cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}


struct VSIN {
	float4 Pos : POSITION;
};

struct PSIN {
	float4 Pos : SV_POSITION;
};

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex = float4(1, 1, 1, 1);
	return tex;
}