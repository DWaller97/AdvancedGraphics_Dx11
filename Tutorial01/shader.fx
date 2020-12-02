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
	float4 worldPos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
	float3x3 TBN: TEXCOORD1;
	float3 Tangent : TANGENT;
	float3 BiTangent : BINORMAL;
	float3 LightVecTan : POSITION1;
	float3 EyeVecTan : POSITION2;
	float3 CamDir : POSITION3;

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
	output.worldPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	output.Norm = normalize(mul(normalize(input.Norm), World));
	output.Tangent = normalize(mul(float4(normalize(input.Tangent.xyz), 0.0f), World));
	output.BiTangent = normalize(mul(float4(normalize(input.BiTangent.xyz), 0.0f), World));

	float3x3 TBN = float3x3(
		output.Tangent, output.BiTangent, output.Norm
		);
	float3x3 TBNInverse = transpose(TBN);
	output.TBN = TBNInverse;
	float3 lightPosWorld = mul(Lights[0].Direction.xyz, World);

	float3 e = mul(EyePosition.xyz, TBNInverse);
	float3 p = mul(output.worldPos.xyz, TBNInverse);
	float3 vertexToEye = e - p;
	float3 vertexToLight = lightPosWorld - output.worldPos;
	output.EyeVecTan = vertexToEye;
	output.LightVecTan = mul(vertexToLight.xyz, TBNInverse);

	// multiply the normal by the world transform (to go from model space to world space)
	output.Norm = mul(output.Norm, TBNInverse);
	output.Tex = input.Tex;
	return output;
}


float2 ParallaxMapping(float2 _texCoords, float3 _viewDir) { //SampleGrad for more advanced stuff?
	float height = txParallax.Sample(samLinear, _texCoords).x;
	height *= 0.0125f;
	float2 p = _viewDir.xy / _viewDir.z * height;
	return _texCoords - p;
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
	texCoords = ParallaxMapping(IN.Tex, IN.EyeVecTan);

	//float heightMapScale = 0.002f;
	//float parallaxLimit = -length(IN.EyeVecTan.xy) / IN.EyeVecTan.z;
	//parallaxLimit *= heightMapScale;

	//float2 offsetDir = normalize(IN.EyeVecTan.xy);
	//float2 maxOffset = offsetDir * parallaxLimit;
	//int minSamples = 5;
	//int maxSamples = 20;
	//int samples = lerp(maxSamples, minSamples, abs(dot(IN.Norm, IN.EyeVecTan)));

	//float stepSize = 1 / float(samples);

	//float2 dx = ddx(IN.Tex);
	//float2 dy = ddy(IN.Tex);

	//float currRayHeight = 1.0f;
	//float2 currOffset = float2(0, 0);
	//float2 lastOffset = float2(0, 0);
	//float currSampledHeight = 1.0f;
	//float lastSampledHeight = 1.0f;
	//int currSample = 0;

	//while (currSample < samples) {
	//	currSampledHeight = txParallax.SampleGrad(samLinear, IN.Tex + currOffset, dx, dy).a;
	//	if (currSampledHeight > currRayHeight) {
	//		float d1 = currSampledHeight - currRayHeight;
	//		float d2 = (currRayHeight + stepSize) - lastSampledHeight;
	//		float ratio = d1 / (d1 + d2);
	//		currOffset = (ratio)*lastOffset + (1.0 - ratio) * currOffset;
	//		currSample += 1;
	//	}
	//	else {
	//		currSample += 1;
	//		currRayHeight -= stepSize;
	//		lastOffset = currOffset;
	//		currOffset += stepSize * maxOffset;
	//		lastSampledHeight = currSampledHeight;
	//	}
	//}
	//currOffset *= 2;
	//texCoords = IN.Tex + currOffset;

	if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0 || texCoords.y < 0)
		discard;



	texNormal = txNormal.Sample(samLinear, texCoords);
	//OpenGL normal correction
	texNormal = texNormal * 2.0f - 1.0f;

	//texNormal.y = 1 - texNormal.y;


	//texNormal = float4(mul(IN.TBN, texNormal), 1.0f);

		if (Material.UseTexture)
	{
		texColor = txDiffuse.Sample(samLinear, texCoords);
	}

	float4 normal = float4(((texNormal.x * IN.Tangent) + (texNormal.y * IN.BiTangent) + (texNormal.z * IN.Norm)).xyz, 1.0f);

	LightingResult lit = ComputeLighting(IN.worldPos, normal, IN.LightVecTan, IN.EyeVecTan);

	float4 emissive = Material.Emissive;
	float4 ambient = Material.Ambient * GlobalAmbient;
	float4 diffuse = Material.Diffuse * lit.Diffuse;
	float4 specular = Material.Specular * lit.Specular;


	float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;

	return finalColor;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target
{
	return vOutputColor;
}
