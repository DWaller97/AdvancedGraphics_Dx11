#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class Light
{
public:
	enum LightType
	{
		DirectionalLight = 0,
		PointLight = 1,
		SpotLight = 2
	};

	Light() : position(0.0f, 0.0f, 0.0f, 1.0f),
		rotation(0.0f, 0.0f, 1.0f, 0.0f),
		colour(1.0f, 1.0f, 1.0f, 1.0f),
		spotAngle(DirectX::XM_PIDIV2),
		constantAttenuation(1.0f),
		linearAttenuation(0.0f),
		quadraticAttenuation(0.0f),
		lightType(PointLight),
		enabled(0)
	{};

	XMFLOAT4 position;
	XMFLOAT4 rotation;
	XMFLOAT4 colour;
	float spotAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	LightType lightType;
	bool enabled;
	int padding, padding2;
};

