#include "GameObject.h"

using namespace std;
using namespace DirectX;

GameObject::GameObject()
{
	SetWorldMatrix(new XMFLOAT4X4());
}

GameObject::~GameObject()
{
	Release();
}

void GameObject::Update(float t)
{
	XMMATRIX mTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(m_world, world);
}

void GameObject::SetPosition(XMFLOAT3 _position)
{
	m_position = _position;
}

void GameObject::SetShaders(ShaderData _shaderData)
{
	m_pixelShader = _shaderData._pixelShader;
	m_vertexShader = _shaderData._vertexShader;
	m_hullShader = _shaderData._hullShader;
	m_domainShader = _shaderData._domainShader;
	m_inputLayout = _shaderData._inputLayout;

}

void GameObject::SetMesh(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords)
{
	m_mesh = OBJLoader::Load(filename, _pd3dDevice, invertTexCoords);
	NUM_VERTICES = m_mesh.VertexCount;
	NUM_INDICES = m_mesh.IndexCount;
}

void GameObject::SetAlbedoTexture(ID3D11ShaderResourceView* _resourceView)
{
	m_albedoTexture = _resourceView;
}

void GameObject::SetAlbedoTexture(const wchar_t* _filePath, ID3D11Device* _device)
{
	CreateDDSTextureFromFile(_device, _filePath, nullptr, &m_albedoTexture);

}

void GameObject::SetNormalTexture(ID3D11ShaderResourceView* _resourceView)
{
	m_normalTexture = _resourceView;
}

void GameObject::SetNormalTexture(const wchar_t* _filePath, ID3D11Device* _device)
{
	CreateDDSTextureFromFile(_device, _filePath, nullptr, &m_normalTexture);
}

void GameObject::SetOcclusionTexture(ID3D11ShaderResourceView* _resourceView)
{
	m_parallaxTexture = _resourceView;
}

void GameObject::SetOcclusionTexture(const wchar_t* _filePath, ID3D11Device* _device)
{
	CreateDDSTextureFromFile(_device, _filePath, nullptr, &m_parallaxTexture);
}

void GameObject::Release()
{
	if (m_mesh.VertexBuffer)
		m_mesh.VertexBuffer->Release();

	if (m_mesh.IndexBuffer)
		m_mesh.IndexBuffer->Release();

	if (m_constantBuffer)
		m_constantBuffer->Release();

	if (m_tesselationBuffer)
		m_tesselationBuffer->Release();

	if (m_textureResourceView)
		m_textureResourceView->Release();

	if (m_samplerLinear)
		m_samplerLinear->Release();


}

void GameObject::SetWorldMatrix(XMFLOAT4X4* _world)
{
	m_world = _world;
}



void GameObject::CalculateTangentBinormal2(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal)
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
void GameObject::CalculateModelVectors(SimpleVertex* _vertices, int _vertexCount)
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