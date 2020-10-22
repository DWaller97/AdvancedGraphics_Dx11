struct Light {
	float4 Position;
	float4 Direction;
	float4 Colour;
	float SpotAngle;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
};

cbuffer Lighting : register(b2) {
	float4 Eye;
	float4 GlobalAmbient;
	Light LightData : LIGHT;
}

cbuffer WorldViewProjection : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}


struct VS_INPUT {
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};

struct PS_INPUT {
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};

PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;
	output.Position = mul(input.Position, World);
	output.WorldPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	output.TexCoord = input.TexCoord;

	output.Normal = mul(float4(input.Normal, 1), World).xyz;
	output.Normal = normalize(output.Normal);

	output.Tangent = mul(input.Tangent, (float3x3) World);
	output.Tangent = normalize(output.Tangent);

	output.BiNormal = mul(input.BiNormal, (float3x3) World);
	output.BiNormal = normalize(output.BiNormal);

	//float4x4 TBN4 = { output.Tangent.x, output.Tangent.y, output.Tangent.z, 0,
	//				output.BiNormal.x, output.BiNormal.y, output.BiNormal.z, 0,
	//				output.Normal.x, output.Normal.y, output.Normal.z, 0,
	//				0, 0, 0, 1 };
	//output.Eye = mul(input.Eye, TBN4);

	return output;
}