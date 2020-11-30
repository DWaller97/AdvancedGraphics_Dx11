#include "DrawableObjectCube.h"


DrawableObjectCube::DrawableObjectCube()
{
	SetWorldMatrix(new XMFLOAT4X4());
	NUM_VERTICES = 36;

}

DrawableObjectCube::~DrawableObjectCube()
{
	delete m_World;
	m_World = nullptr;
	Release();
}

HRESULT DrawableObjectCube::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)  {
	// Create vertex buffer



	SimpleVertex vertices[24] =
	{
		//Bottom
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},

		//Top
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},
		//Left
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},
		//Right
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},
		//Front
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},
		//Back
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0)},
		{ XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0)},
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1)},
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1)},
	};

	WORD indices[36] =
	{
		0,3,2,
		2,1,0,

		4,7,6,
		6,5,4,

		8,11,10,
		10,9,8,

		12,15,14,
		14,13,12,

		16,19,18,
		18,17,16,

		20,23,22,
		22,21,20,
	};

	CalculateModelVectors(vertices, 24);


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	HRESULT hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CameraBuffer);      
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCameraBuffer);
	if (FAILED(hr))
		return hr;

	// Create the material constant buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer);
	if (FAILED(hr))
		return hr;

	// Create the camera constant buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer);
	if (FAILED(hr))
		return hr;

	//// Create the light constant buffer
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = 0;
	//hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pLightConstantBuffer);
	//if (FAILED(hr))
	//	return hr;


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;  
	bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.IndexBuffer);
	if (FAILED(hr)) 
		return hr;
	DrawableGameObject::InitMesh(pd3dDevice, pContext);

} 

void DrawableObjectCube::Update(float t)
{
	m_parallaxBias = 1.0f;
	m_parallaxScale = 1.0f;
	if(spinning){
		static float spin = 0;
		spin += 0.001f;
		if (spin > 360)
			spin = 0;
		XMMATRIX mSpin = XMMatrixRotationY(spin * (t));

	}
	// Cube:  Rotate around origin

	XMMATRIX mTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(m_World, world);
}

void DrawableObjectCube::SetSpin(bool spin)
{
	spinning = spin;
}