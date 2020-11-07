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

//cbuffer Parallax : register(b3) {
//	float parallaxBias;
//	float parallaxScale;
//	float2 align;
//}

struct PS_INPUT {
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION8;
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



Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txParallax : register(t2);
SamplerState samLinear : register(s0);

float2 ParallaxMapping(float2 _texCoords, float3 _viewDir) {
	float height = txParallax.Sample(samLinear, _texCoords).x;
	float2 p = _viewDir.xy / _viewDir.z * (height * 1);
	return _texCoords - p;
}


float4 main(PS_INPUT input) : SV_TARGET
{

	float4 parallax = txParallax.Sample(samLinear, input.TexCoord);
	float3 viewDirection = input.EyePosTS - input.WorldPosTS;
	viewDirection = normalize(viewDirection);
	//float2 texCoords = ParallaxMapping(input.TexCoord, viewDirection);

	float4 colour = float4(0, 0, 0, 0);
	float4 txColour = txDiffuse.Sample(samLinear, input.TexCoord);

	float4 normalMap = txNormal.Sample(samLinear, input.TexCoord);
	normalMap = normalMap * 2.0f - 1.0f;
	//OpenGL Normal Map
	//normalMap.g = -normalMap.g;

	float3x3 TBN = float3x3(input.Tangent, input.BiNormal, input.Normal);

	input.Normal = normalize(mul(input.Normal, TBN));
	float3 normal = normalize(input.Normal);
	/*normal = (normalMap.x * input.Tangent) + (normalMap.y * input.BiNormal) + (normalMap.z * input.Normal);
	normal = normalize(normal);*/



	float3 eyeVertex = normalize(input.Eye - input.WorldPosition).xyz;
	float3 lightDirToVertex = (input.WorldPosTS - input.Light).xyz;
	float dist = length(lightDirToVertex);
	lightDirToVertex = lightDirToVertex / dist;

	float4 diffuse;
	float4 specular;
	float4 ambient = Material.Ambient * GlobalAmbient;

	float3 vertexToLight = (LightData.Position - input.WorldPosTS).xyz;
	dist = length(vertexToLight);
	vertexToLight = vertexToLight / dist;

	float attenuation = 1.0f / (LightData.ConstantAttenuation + LightData.LinearAttenuation * dist + LightData.QuadraticAttenuation * dist * dist);
	diffuse = (LightData.Colour * max(0, dot(normal, vertexToLight))) * attenuation;
	eyeVertex = normalize(eyeVertex);
	float4 lightDirection = float4(normalize(-lightDirToVertex), 1);

	float lightIntensity = saturate(dot(normal, lightDirection));
	if (lightIntensity > 0) {
		float3 reflection = reflect(normalize(lightDirection), normalize(normal))/*normalize(2 * lightIntensity * normal - lightDirection)*/;
		specular = pow(saturate(dot(reflection, eyeVertex)), Material.SpecularPower);
	}

	diffuse = Material.Diffuse * diffuse;
	specular = Material.Specular * specular;
	//

	diffuse = saturate(diffuse * lightIntensity);
	float4 final = float4(Material.Emissive + ambient + diffuse + specular)*   txColour;
	return final;
}