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
	float4 WorldPosition : TEXCOORD1;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float3 Eye : POSITION3;
	float3 Light : POSITION4;
	float3 PosTS : POSITION5;
	float3 EyePosTS : POSITION6;
	float3 WorldPosTS : POSITION7;
};


float3 VectorToTangentSpace(float3 vectorV, float3x3 TBN_inv)
{
	// Transform from tangent space to world space.
	float3 tangentSpaceNormal = normalize(mul(vectorV, TBN_inv));
	return tangentSpaceNormal;
}



PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;
	output.Position = mul(input.Position, World);
	float4 worldPos = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	float4 normalWS = mul(input.Normal, World);


	float3 vertexToEye = Eye.xyz - worldPos.xyz;
	float3 vertexToLight = LightData.Position.xyz - worldPos.xyz;



	output.TexCoord = input.TexCoord;

	output.Normal = mul(float4(input.Normal, 1), World).xyz;
	output.Normal = normalize(output.Normal);

	output.Tangent = mul(input.Tangent, (float3x3) World);
	output.Tangent = normalize(output.Tangent);

	output.BiNormal = mul(input.BiNormal, (float3x3) World);
	output.BiNormal = normalize(output.BiNormal);

	float3 T = normalize(mul(input.Tangent, World));
	float3 B = normalize(mul(input.BiNormal, World));
	float3 N = normalize(mul(input.Normal, World));
	float3x3 TBN = float3x3(T, B, N);
	float3x3 TBNInverse = transpose(TBN);
	
	output.Eye = VectorToTangentSpace(vertexToEye.xyz, TBNInverse);
	output.WorldPosition = worldPos;
	output.Light = VectorToTangentSpace(vertexToLight.xyz, TBNInverse);
	output.Normal = VectorToTangentSpace(normalWS.xyz, TBNInverse);
	output.EyePosTS = VectorToTangentSpace(Eye.xyz, TBNInverse);
	output.WorldPosTS = VectorToTangentSpace(worldPos.xyz, TBNInverse);


	return output;
}