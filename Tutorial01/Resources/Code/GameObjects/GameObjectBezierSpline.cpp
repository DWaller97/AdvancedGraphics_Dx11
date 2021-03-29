#include "GameObjectBezierSpline.h"
GameObjectBezierSpline::GameObjectBezierSpline()
{
	NUM_VERTICES = 4;
	NUM_INDICES = 0;
}

GameObjectBezierSpline::~GameObjectBezierSpline()
{
}

HRESULT GameObjectBezierSpline::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	SimpleVertex linePoints[] =
	{

		{ XMFLOAT3(0, -1.0f, 0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0.0f, 1.0f, 1.0f) , XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(2.0f, 0, 0), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(3.0f, 1.0f, 0), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};
	SimpleVertex* linePoints2 = new SimpleVertex[4 * NUM_VERTICES];
	int k = 0;
	for (int i = 0; i < NUM_VERTICES; i++) {
		float posX = linePoints[i].pos.x;
		float posY = linePoints[i].pos.y;
		for (float j = 0.0f; j < 1; j += 0.25f, k++) {
			SimpleVertex* v = new SimpleVertex();
			v->pos.x = (1 - j) * (1 - j) * posX + 2 * (1 - j) * j * posX + (j * j) * posX;
			v->pos.y = (1 - j) * (1 - j) * posY + 2 * (1 - j) * j * posY + (j * j) * posY;
			linePoints2[k] = *v;
		}
	}


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = linePoints2;
	HRESULT hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_constantBuffer);
	if (FAILED(hr))
		return hr;

}

void GameObjectBezierSpline::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void GameObjectBezierSpline::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);

	// Set vertex buffer
	
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_mesh.VertexBuffer, &stride, &offset);

	pContext->IASetInputLayout(m_inputLayout);
	// Render the cube
	pContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);
	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}