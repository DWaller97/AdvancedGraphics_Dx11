Texture2D tx : register(t0);
Texture2D txNormal : register(t1);
SamplerState samLinear : register(s0);

struct _Material
{
	float4  Emissive;       // 16 bytes
							//----------------------------------- (16 byte boundary)
	float4  Ambient;        // 16 bytes
							//------------------------------------(16 byte boundary)
	float4  Diffuse;        // 16 bytes
							//----------------------------------- (16 byte boundary)
	float4  Specular;       // 16 bytes
							//----------------------------------- (16 byte boundary)
	float   SpecularPower;  // 4 bytes
	bool    UseTexture;     // 4 bytes
	float2  Padding;        // 8 bytes
							//----------------------------------- (16 byte boundary)
};  // Total:               // 80 bytes ( 5 * 16 )

cbuffer MaterialProperties : register(b1)
{
	_Material Material;
};
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

struct VSOUT {
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

float4 Diffuse(VSOUT input) {
	float4 final = float4(0, 0, 0, 0);
	float3 lightToPixel = LightData.Position - input.WorldPosition;
	float l = length(lightToPixel);
	lightToPixel = lightToPixel / l;
	float brightness = dot(lightToPixel, input.Normal);
	if (brightness > 0.0f) {
		final += LightData.Colour;
		final /= LightData.ConstantAttenuation + (LightData.LinearAttenuation * l) + (LightData.QuadraticAttenuation * (l * l));
		final *= pow(max(dot(-lightToPixel, LightData.Direction), 0.0f), LightData.SpotAngle);
	}
	return final;
}

float4 main(VSOUT input) : SV_TARGET
{
	float4 matAmbient = Material.Ambient;
	float4 matDiffuse = Material.Diffuse;
	float4 matEmissive = Material.Emissive;
	float4 mat = matAmbient + matEmissive;
	float4 txColour = tx.Sample(samLinear, input.TexCoord);
	float3 normal = txNormal.Sample(samLinear, input.TexCoord);
	txColour = txColour * matAmbient * LightData.Colour;
	float4 diffuse = Diffuse(input);
	
	diffuse = saturate(max(GlobalAmbient, dot(LightData.Direction, input.Normal))) * matDiffuse + diffuse;
	return saturate(mat + diffuse) * txColour;
}