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



struct PSIN {
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float2 Tex : TEXCOORD;
};

SamplerState samLinear : register(s0);
Texture2D txAlbedo0 : register(t0);
Texture2D txAlbedo1 : register(t1);
Texture2D txAlbedo2 : register(t2);
Texture2D txAlbedo3 : register(t3);
Texture2D txAlbedo4 : register(t4);

cbuffer PixelTextureHeights : register(b0){
	float4 AlbedoZeroToThreeHeight;
};

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex;
	if(IN.WorldPos.y <= AlbedoZeroToThreeHeight.x)
		tex = txAlbedo0.Sample(samLinear, IN.Tex);
	if (IN.WorldPos.y <= AlbedoZeroToThreeHeight.y && IN.WorldPos.y > AlbedoZeroToThreeHeight.x)
		tex = txAlbedo1.Sample(samLinear, IN.Tex);
	if (IN.WorldPos.y <= AlbedoZeroToThreeHeight.z && IN.WorldPos.y > AlbedoZeroToThreeHeight.y)
		tex = txAlbedo2.Sample(samLinear, IN.Tex);
	if (IN.WorldPos.y <= AlbedoZeroToThreeHeight.w && IN.WorldPos.y > AlbedoZeroToThreeHeight.z)
		tex = txAlbedo3.Sample(samLinear, IN.Tex);
	if (IN.WorldPos.y > AlbedoZeroToThreeHeight.w)
		tex = txAlbedo4.Sample(samLinear, IN.Tex);
	return tex;
}