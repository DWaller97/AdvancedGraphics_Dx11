
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


float4 DoDiffuse(Light light, float3 L, float3 N)
{
	float NdotL = max(0, dot(N, L));
	return light.Color * NdotL;
}

float4 DoSpecular(Light lightObject, float3 vertexToEye, float3 lightDirectionToVertex, float3 Normal)
{
	float4 lightDir = float4(normalize(-lightDirectionToVertex), 1);
	vertexToEye = normalize(vertexToEye);

	float lightIntensity = saturate(dot(Normal, lightDir));
	float4 specular = float4(0, 0, 0, 0);
	if (lightIntensity > 0.0f)
	{

		float3  reflection = normalize(2 * lightIntensity * Normal - lightDir);
		specular = pow(saturate(dot(reflection, vertexToEye)), Material.SpecularPower); // 32 = specular power
	}

	return specular;
}

float DoAttenuation(Light light, float d)
{
	return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

LightingResult DoPointLight(Light light, float3 vertexToEye, float4 vertexPos, float3 N, float3 vertToLight)
{
	LightingResult result;

	float3 lightToVertex = (vertexPos - light.Position);
	float distance = length(lightToVertex);

	float3 vertexToLight = light.Position + (light.Direction * distance);
	distance = length(vertexToLight);

	float attenuation = DoAttenuation(light, distance);


	result.Diffuse = DoDiffuse(light, vertexToLight, N) * attenuation;
	result.Specular = DoSpecular(light, vertexToEye, -vertexToLight, N) * attenuation;

	return result;
}

LightingResult ComputeLighting(float4 vertexPos, float3 N, float3 lightVec, float3 eye)
{
	float3 vertexToEye = eye - vertexPos;

	LightingResult totalResult = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };
	Light light;
	light = Lights[0];
	[unroll]
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		LightingResult result = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };

		if (!Lights[i].Enabled)
			continue;

		result = DoPointLight(light, vertexToEye, vertexPos, N, lightVec);
		totalResult.Diffuse += result.Diffuse;
		totalResult.Specular += result.Specular;
	}

	totalResult.Diffuse = saturate(totalResult.Diffuse);
	totalResult.Specular = saturate(totalResult.Specular);

	return totalResult;
}


Texture2D txAlbedo : register(t0);
Texture2D txNormal : register(t1);
Texture2D txPosition : register(t2);

SamplerState samLinear : register(s0);

PSIN VS(VSIN IN) {
	PSIN output = (PSIN)0;
	output.Pos = mul(IN.Pos, World);
	output.WorldPos = output.Pos;
	//output.Pos = mul(mul(output.Pos, View), Projection);

	float3 lw = mul(Lights[0].Position, World);
	output.Tex = IN.Tex;
	return output;
}

float4 PS(PSIN IN) : SV_TARGET
{
	float4 texColour = { 0, 0, 0, 0 };
	float4 texNormal = { 1, 1, 1, 1 };
	float4 texPosition = { 0, 0, 0, 0 };
	texNormal = txNormal.Sample(samLinear, IN.Tex);
	texNormal = texNormal * 2.0f - 1.0f;
	texColour = txAlbedo.Sample(samLinear, IN.Tex);
	texPosition = txPosition.Sample(samLinear, IN.Tex);
	LightingResult lit = ComputeLighting(texPosition, texNormal, normalize(Lights[0].Direction), normalize(EyePosition - texPosition));
	float4 emissive = Material.Emissive;
	float4 ambient = Material.Ambient * GlobalAmbient;
	float4 diffuse = Material.Diffuse * lit.Diffuse;
	float4 specular = Material.Specular * lit.Specular;
	float4 finalColour = (emissive + ambient + diffuse + specular) * texColour;
	return finalColour;
}