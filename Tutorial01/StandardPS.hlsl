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


struct PS_INPUT {
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};



Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
SamplerState samLinear : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
	float4 colour = float4(0, 0, 0, 0);
	


	float4 lightDir = LightData.Direction;
	lightDir = -lightDir;
	
	float4 txColour = txDiffuse.Sample(samLinear, input.TexCoord);
	
	float4 normalMap = txNormal.Sample(samLinear, input.TexCoord);
	normalMap = normalMap * 2.0f - 1.0f;
	//OpenGL Normal Map
	normalMap.g = -normalMap.g;

	float3x3 TBN = float3x3(input.Tangent, input.BiNormal, input.Normal);
	input.Normal = normalize(mul(normalMap, TBN));

	float3 normal = normalize(input.Normal);
	normal = (normalMap.x * input.Tangent) + (normalMap.y * input.BiNormal) + (normalMap.z * input.Normal);
	normal = normalize(normal);
	


	float3 eyeVertex = normalize(Eye - input.WorldPosition).xyz;
	float3 lightDirToVertex = (input.WorldPosition - LightData.Position).xyz;
	float dist = length(lightDirToVertex);
	lightDirToVertex = lightDirToVertex / dist;

	float4 diffuse;
	float4 specular;
	float4 ambient = Material.Ambient * GlobalAmbient;

	float3 vertexToLight = (LightData.Position - input.WorldPosition).xyz;
	dist = length(vertexToLight);
	vertexToLight = vertexToLight / dist;

	float attenuation = 1.0f / (LightData.ConstantAttenuation + LightData.LinearAttenuation * dist + LightData.QuadraticAttenuation * dist * dist);
	diffuse = (LightData.Colour * max(0, dot(normal, vertexToLight))) * attenuation;
	eyeVertex = normalize(eyeVertex);
	float4 lightDirection = float4(normalize(-lightDirToVertex), 1);

	float lightIntensity = saturate(dot(normal, lightDirection));
	if (lightIntensity > 0) {
		float3 reflection = normalize(2 * lightIntensity * normal - lightDirection);
		specular = pow(saturate(dot(reflection, eyeVertex)), Material.SpecularPower);
	}
	
	diffuse = Material.Diffuse * diffuse;
	specular = Material.Specular * specular;
	//
	
	colour = saturate(diffuse * lightIntensity);
	float4 final = (Material.Emissive + ambient + colour) * txColour;
	return final;
}