
#define MAX_LIGHTS 1


cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

cbuffer SettingsBuffer : register(b1) {
	bool InvertColours;
	bool padding;
	bool padding1;
	bool padding2;
}

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

cbuffer MaterialProperties : register(b2)
{
	_Material Material;
};

struct Light
{
	float4      Position;               // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4      Direction;              // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4      Color;                  // 16 bytes
										//----------------------------------- (16 byte boundary)
	float       SpotAngle;              // 4 bytes
	float       ConstantAttenuation;    // 4 bytes
	float       LinearAttenuation;      // 4 bytes
	float       QuadraticAttenuation;   // 4 bytes
										//----------------------------------- (16 byte boundary)
	int         LightType;              // 4 bytes
	bool        Enabled;                // 4 bytes
	int2        Padding;                // 8 bytes
										//----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (5 * 16)

cbuffer LightProperties : register(b3)
{
	float4 EyePosition;                 // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4 GlobalAmbient;               // 16 bytes
										//----------------------------------- (16 byte boundary)
	Light Lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};

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

Texture2D txAlbedo : register(t0);
Texture2D txNormal : register(t1);
Texture2D txPosition : register(t2);

SamplerState samLinear : register(s0);

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.WorldPos = output.Pos;
	//output.Pos = mul(output.Pos, View);
	//output.Pos = mul(output.Pos, Projection);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	float4 tex = float4(txAlbedo.Sample(samLinear, IN.Tex));
	return tex;
}