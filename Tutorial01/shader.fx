//--------------------------------------------------------------------------------------
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

// the lighting equations in this code have been taken from https://www.3dgep.com/texturing-lighting-directx-11/
// with some modifications by David White

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txParallax : register(t2);
SamplerState samLinear : register(s0);

#define MAX_LIGHTS 1
// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

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

cbuffer LightProperties : register(b2)
{
	float4 EyePosition;                 // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4 GlobalAmbient;               // 16 bytes
										//----------------------------------- (16 byte boundary)
	Light Lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};

//--------------------------------------------------------------------------------------


cbuffer CameraProperties : register(b3) {
	float3 CameraPosition;
	float padding;
	float3 CameraDirection;
	float padding2;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 Tangent : TANGENT;
	float3 BiTangent : BINORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 NormTan : NORMAL;
	float3 EyeTan : POSITION4;
	float3 PosTan : POSITION5;
	float3 LightPosTan : POSITION6;
	float2 Tex : TEXCOORD0;
	float3 Tangent : TANGENT;
	float3 BiTangent : BINORMAL;
	float3 LightVecTan : POSITION1;
	float3 EyeVecTan : POSITION2;
	float3 CamDir : POSITION3;
	float3x3 TBN : POSITION7;
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
	float3 vertexToEye = eye;

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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.WorldPos = output.Pos;
	output.Pos = mul(mul(output.Pos, View), Projection);

	output.Tangent = input.Tangent.xyz;
	output.BiTangent = input.BiTangent.xyz;

	float3x3 TBN = transpose(float3x3(output.Tangent, output.BiTangent, input.Norm));
	output.TBN = TBN;
	float3 lw = mul(Lights[0].Position, World);
	float3 e = mul(EyePosition.xyz, TBN);
	float3 p = mul(output.WorldPos.xyz, TBN);
	float3 l = mul(lw.xyz, TBN);
	//output.EyeVecTan = e - p;
	output.LightVecTan = l - p;
	output.LightPosTan = l;
	output.EyeTan = e;
	output.PosTan = p;
	output.NormTan = mul(input.Norm, TBN);
	output.Tex = input.Tex;
	return output;
}


float2 SteepParallaxMapping(float2 _texCoords, float3 _viewDir, float3 _norm) {
	int minSamples = 5;
	int maxSamples = 20;
	int numSamples = lerp(maxSamples, minSamples, abs(dot(_norm, _viewDir)));
	float layerDepth = 1.0 / numSamples;
	float currLayerDepth = 0.0f;
	float heightScale = 0.05f;
	float2 p = _viewDir.xy * heightScale;
	float2 offset = p / numSamples;
	float2 currCoords = _texCoords;
	float currDepth = txParallax.Sample(samLinear, currCoords);
	float2 dx = ddx(_texCoords);
	float2 dy = ddy(_texCoords);

	while (currLayerDepth < currDepth) {
		currCoords -= offset;
		currDepth = txParallax.SampleGrad(samLinear, currCoords, dx, dy).x;
		currLayerDepth += layerDepth;
	}
	return currCoords;
}

float2 ParallaxOcclusionMapping(float2 _texCoords, float3 _viewDir, float3 _norm) {
	int minSamples = 5;
	int maxSamples = 20;
	int numSamples = lerp(maxSamples, minSamples, abs(dot(float3(0, 0, 1), _viewDir)));
	float layerDepth = 1.0 / numSamples;
	float currLayerDepth = 0.0f;
	float heightScale = 0.05f;
	float2 p = _viewDir.xy * heightScale;
	float2 offset = p / numSamples;
	float2 currCoords = _texCoords;
	float currDepth = txParallax.Sample(samLinear, currCoords);
	float2 dx = ddx(_texCoords);
	float2 dy = ddy(_texCoords);

	float2 prevCoords;
	float afterDepth;
	float prevDepth;
	float weight;
	while (currLayerDepth < currDepth) {

		currCoords -= offset; 
		currDepth = txParallax.SampleGrad(samLinear, currCoords, dx, dy).x;
		currLayerDepth += layerDepth;

	}

	prevCoords = currCoords + offset;
	afterDepth = currDepth - currLayerDepth;

	prevDepth = txParallax.Sample(samLinear, prevCoords).x - currLayerDepth + layerDepth;
	weight = afterDepth / (afterDepth - prevDepth);

	return prevCoords * weight + currCoords * (1.0 - weight);
}

float SelfShadow(float2 _texCoords, float3 _lightVec, float3 _norm) {
	int numSamples = 4;
	float layerDepth = 1.0 / numSamples;
	float heightScale = 0.05f;
	float2 offset = heightScale * _lightVec.xy / _lightVec.z / numSamples;
	float2 currCoords = _texCoords;
	float depthMap = txParallax.Sample(samLinear, currCoords);
	float currLayerDepth = depthMap / numSamples;
	float2 dx = ddx(depthMap);
	float2 dy = ddy(depthMap);
	float samplesUnderSurface = 0;
	float shadowMultiplier = 0;
	float stepIndex = 1;
	while (currLayerDepth > 0) {
		if (depthMap < currLayerDepth) {
			samplesUnderSurface++;
			float newShadow = (currLayerDepth - depthMap) * (1.0 - stepIndex / numSamples);
			shadowMultiplier = max(shadowMultiplier, newShadow);
		}
		stepIndex++;
		currLayerDepth -= layerDepth;
		currCoords += offset;
		depthMap = txParallax.SampleGrad(samLinear, currCoords, dx, dy).x;
	}

	if (samplesUnderSurface < 1) {
		shadowMultiplier = 1;
	}
	else {
		shadowMultiplier = 1.0 - shadowMultiplier;
	}

	return shadowMultiplier;

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT IN) : SV_TARGET
{
	float4 texColor = { 0, 0, 0, 0 };
	float4 texNormal = { 1, 1, 1, 1 };
	float4 texParallax = { 1, 1, 1, 1 };
	float2 texCoords = IN.Tex;

	float3 EyeVecTan = normalize(mul(EyePosition, IN.TBN) - IN.PosTan);
	float3 LightTan = normalize(mul(Lights[0].Position, IN.TBN) - IN.PosTan);//normalize(IN.LightPosTan - IN.PosTan);

	texCoords = ParallaxOcclusionMapping(IN.Tex, EyeVecTan, IN.NormTan);
	float shadowFactor = 1;
	

	if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0 || texCoords.y < 0)
		discard;



	texNormal = txNormal.Sample(samLinear, texCoords);
	//OpenGL normal correction
	texNormal = texNormal * 2.0f - 1.0f;

	//texNormal.y = 1 - texNormal.y;



		if (Material.UseTexture)
	{
		texColor = txDiffuse.Sample(samLinear, texCoords);
	}

	LightingResult lit = ComputeLighting(IN.WorldPos, texNormal, LightTan, IN.EyeVecTan);

	float4 emissive = Material.Emissive;
	float4 ambient = Material.Ambient * GlobalAmbient;
	float4 diffuse = Material.Diffuse * lit.Diffuse;
	float4 specular = Material.Specular * lit.Specular;


	shadowFactor = SelfShadow(texCoords, LightTan, texNormal);
	float4 finalColor = (emissive + ambient + (shadowFactor  * diffuse) + (shadowFactor * specular)) * texColor;

	return finalColor;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target
{
	return vOutputColor;
}
