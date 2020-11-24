#include "DrawableGameObject.h"

using namespace std;
using namespace DirectX;

DrawableGameObject::DrawableGameObject()
{
	SetWorldMatrix(new XMFLOAT4X4());
}

DrawableGameObject::~DrawableGameObject()
{
}

HRESULT DrawableGameObject::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd = {};


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
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

	MaterialPropertiesConstantBuffer redPlasticMaterial;
	redPlasticMaterial.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	redPlasticMaterial.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial.Material.SpecularPower = 32.0f;
	redPlasticMaterial.Material.UseTexture = true;
	pContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, nullptr, &redPlasticMaterial, 0, 0);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);


	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Crate_COLOR.dds", nullptr, &m_albedoTexture);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\conenormal.dds", nullptr, &m_normalTexture);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\conedisp.dds", nullptr, &m_parallaxTexture);
	if (FAILED(hr))
		return hr;
}

void DrawableGameObject::Update(float t)
{
	XMMATRIX mTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(m_World, world);
}

void DrawableGameObject::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_World));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	//Camera* currCam = CameraManager::GetCurrCamera();
	//CameraBuffer cb2;
	//cb2.cameraDirection = currCam->GetPosition + currCam->GetFor;
	//cb2.cameraPosition = currCam->GetPosition();

	//if(m_pCameraBuffer)
	//	pContext->UpdateSubresource(m_pCameraBuffer, 3, nullptr, &cb2, 0, 0);

	//ConstantBuffer cb2;
	//cb2.parallaxBias = m_parallaxBias;
	//cb2.parallaxScale = m_parallaxScale;
	//pContext->UpdateSubresource(m_parallaxBuffer, 0, nullptr, &cb2, 0, 0);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	if(m_inputLayout != nullptr)
		pContext->IASetInputLayout(m_inputLayout);
	// Render the cube
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->PSSetShader(pixelShader, nullptr, 0);

	pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);
	pContext->PSSetConstantBuffers(2, 1, &lightConstantBuffer);
	//pContext->PSSetConstantBuffers(3, 1, &m_pCameraBuffer);
	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetShaderResources(1, 1, &m_normalTexture);
	pContext->PSSetShaderResources(2, 1, &m_parallaxTexture);

	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
}

void DrawableGameObject::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void DrawableGameObject::SetShaders(ShaderData shaderData)
{
	pixelShader = shaderData._pixelShader;
	vertexShader = shaderData._vertexShader;
	m_inputLayout = shaderData._inputLayout;

}

void DrawableGameObject::SetShader(ID3D11PixelShader* _pixelShader)
{
	pixelShader = _pixelShader;
}

void DrawableGameObject::SetShader(ID3D11VertexShader* _vertexShader)
{
	vertexShader = _vertexShader;
}

void DrawableGameObject::SetShaders(ID3D11VertexShader* _vertexShader, ID3D11PixelShader* _pixelShader)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
}

void DrawableGameObject::SetParallaxScale(float _scale)
{
	m_parallaxScale = _scale;
}

void DrawableGameObject::SetParallaxBias(float _bias)
{
	m_parallaxBias = _bias;
}

void DrawableGameObject::SetMesh(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords)
{
	mesh = OBJLoader::Load(filename, _pd3dDevice, invertTexCoords);
	NUM_VERTICES = mesh.VertexCount;
}

void DrawableGameObject::Release()
{
	if (mesh.VertexBuffer)
		mesh.VertexBuffer->Release();

	if (mesh.IndexBuffer)
		mesh.IndexBuffer->Release();

	if (m_pTextureResourceView)
		m_pTextureResourceView->Release();

	if (m_pSamplerLinear)
		m_pSamplerLinear->Release();
}

void DrawableGameObject::SetWorldMatrix(XMFLOAT4X4* world)
{
	m_World = world;
}



void DrawableGameObject::CalculateTangentBinormal2(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	// http://softimage.wiki.softimage.com/xsidocs/tex_tangents_binormals_AboutTangentsandBinormals.html
	XMFLOAT3 edge1(v1.pos.x - v0.pos.x, v1.pos.y - v0.pos.y, v1.pos.z - v0.pos.z);
	XMFLOAT3 edge2(v2.pos.x - v0.pos.x, v2.pos.y - v0.pos.y, v2.pos.z - v0.pos.z);

	XMFLOAT2 deltaUV1(v1.texCoord.x - v0.texCoord.x, v1.texCoord.y - v0.texCoord.y);
	XMFLOAT2 deltaUV2(v2.texCoord.x - v0.texCoord.x, v2.texCoord.y - v0.texCoord.y);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	XMVECTOR tn = XMLoadFloat3(&tangent);
	tn = XMVector3Normalize(tn);
	XMStoreFloat3(&tangent, tn);

	binormal.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	binormal.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	binormal.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	tn = XMLoadFloat3(&binormal);
	tn = XMVector3Normalize(tn);
	XMStoreFloat3(&binormal, tn);


	XMVECTOR vv0 = XMLoadFloat3(&v0.pos);
	XMVECTOR vv1 = XMLoadFloat3(&v1.pos);
	XMVECTOR vv2 = XMLoadFloat3(&v2.pos);

	XMVECTOR e0 = vv1 - vv0;
	XMVECTOR e1 = vv2 - vv0;

	XMVECTOR e01cross = XMVector3Cross(e0, e1);
	e01cross = XMVector3Normalize(e01cross);
	XMFLOAT3 normalOut;
	XMStoreFloat3(&normalOut, e01cross);
	normal = normalOut;

	return;
}
// NOTE!! - this assumes each face is using its own vertices (no shared vertices)
// so the index file would look like 0,1,2,3,4 and so on
// it won't work with shared vertices as the tangent / binormal for a vertex is related to a specific face
// REFERENCE this has largely been modified from "Mathematics for 3D Game Programmming and Computer Graphics" by Eric Lengyel
void DrawableGameObject::CalculateModelVectors(SimpleVertex* _vertices, int _vertexCount)
{
	int faceCount, i, index;
	SimpleVertex vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = _vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three _vertices for this face from the model.
		vertex1.pos.x = _vertices[index].pos.x;
		vertex1.pos.y = _vertices[index].pos.y;
		vertex1.pos.z = _vertices[index].pos.z;
		vertex1.texCoord.x = _vertices[index].texCoord.x;
		vertex1.texCoord.y = _vertices[index].texCoord.y;
		vertex1.normal.x = _vertices[index].normal.x;
		vertex1.normal.y = _vertices[index].normal.y;
		vertex1.normal.z = _vertices[index].normal.z;
		index++;

		vertex2.pos.x = _vertices[index].pos.x;
		vertex2.pos.y = _vertices[index].pos.y;
		vertex2.pos.z = _vertices[index].pos.z;
		vertex2.texCoord.x = _vertices[index].texCoord.x;
		vertex2.texCoord.y = _vertices[index].texCoord.y;
		vertex2.normal.x = _vertices[index].normal.x;
		vertex2.normal.y = _vertices[index].normal.y;
		vertex2.normal.z = _vertices[index].normal.z;
		index++;

		vertex3.pos.x = _vertices[index].pos.x;
		vertex3.pos.y = _vertices[index].pos.y;
		vertex3.pos.z = _vertices[index].pos.z;
		vertex3.texCoord.x = _vertices[index].texCoord.x;
		vertex3.texCoord.y = _vertices[index].texCoord.y;
		vertex3.normal.x = _vertices[index].normal.x;
		vertex3.normal.y = _vertices[index].normal.y;
		vertex3.normal.z = _vertices[index].normal.z;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal2(vertex1, vertex2, vertex3, normal, tangent, binormal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		_vertices[index - 1].normal.x = normal.x;
		_vertices[index - 1].normal.y = normal.y;
		_vertices[index - 1].normal.z = normal.z;
		_vertices[index - 1].tangent.x = tangent.x;
		_vertices[index - 1].tangent.y = tangent.y;
		_vertices[index - 1].tangent.z = tangent.z;
		_vertices[index - 1].biTangent.x = binormal.x;
		_vertices[index - 1].biTangent.y = binormal.y;
		_vertices[index - 1].biTangent.z = binormal.z;

		_vertices[index - 2].normal.x = normal.x;
		_vertices[index - 2].normal.y = normal.y;
		_vertices[index - 2].normal.z = normal.z;
		_vertices[index - 2].tangent.x = tangent.x;
		_vertices[index - 2].tangent.y = tangent.y;
		_vertices[index - 2].tangent.z = tangent.z;
		_vertices[index - 2].biTangent.x = binormal.x;
		_vertices[index - 2].biTangent.y = binormal.y;
		_vertices[index - 2].biTangent.z = binormal.z;

		_vertices[index - 3].normal.x = normal.x;
		_vertices[index - 3].normal.y = normal.y;
		_vertices[index - 3].normal.z = normal.z;
		_vertices[index - 3].tangent.x = tangent.x;
		_vertices[index - 3].tangent.y = tangent.y;
		_vertices[index - 3].tangent.z = tangent.z;
		_vertices[index - 3].biTangent.x = binormal.x;
		_vertices[index - 3].biTangent.y = binormal.y;
		_vertices[index - 3].biTangent.z = binormal.z;
	}

}