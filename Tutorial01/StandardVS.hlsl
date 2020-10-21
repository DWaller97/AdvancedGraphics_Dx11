cbuffer WorldViewProjection : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}


struct VS_INPUT {
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};

struct PS_INPUT {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};

PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;
	float4 pos = input.Position;
	pos.w = 1.0f;
	pos = mul(pos, World);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

	output.Position = pos;
	output.TexCoord = input.TexCoord;

	output.Normal = mul(input.Normal, (float3x3) World);
	output.Normal = normalize(output.Normal);

	output.Tangent = mul(input.Tangent, (float3x3) World);
	output.Tangent = normalize(output.Tangent);

	output.BiNormal = mul(input.BiNormal, (float3x3) World);
	output.BiNormal = normalize(output.BiNormal);

	return output;
}