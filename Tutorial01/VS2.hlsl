

cbuffer WorldViewProjection : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

struct VSIN {
	float4 Position : SV_POSITION;
	float3 WorldPosition : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VSOUT {
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

VSOUT main(VSIN input)
{
	VSOUT output;
	output.Position = mul(input.Position, World);
	output.WorldPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	output.TexCoord = input.TexCoord;
	output.Normal = normalize(mul(input.Normal, World));
	return output;
}