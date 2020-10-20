#include "DrawableGameObject.h"

using namespace std;
using namespace DirectX;

void DrawableGameObject::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_World));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// Render the cube
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetShader(pixelShader, nullptr, 0);

	pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);
	pContext->PSSetConstantBuffers(2, 1, &lightConstantBuffer);

	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetShaderResources(1, 1, &m_normalTexture);
	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->PSSetSamplers(1, 1, &m_pSamplerNormal);
	pContext->DrawIndexed(36, 0, 0);
}

void DrawableGameObject::SetPosition(XMFLOAT3 position)
{
	m_position = position;
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

void DrawableGameObject::Release()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

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

	// 1. CALCULATE THE NORMAL
	XMVECTOR vv0 = XMLoadFloat3(&v0.pos);
	XMVECTOR vv1 = XMLoadFloat3(&v1.pos);
	XMVECTOR vv2 = XMLoadFloat3(&v2.pos);

	XMVECTOR P = vv1 - vv0;
	XMVECTOR Q = vv2 - vv0;

	XMVECTOR e01cross = XMVector3Cross(P, Q);
	XMFLOAT3 normalOut;
	XMStoreFloat3(&normalOut, e01cross);
	normal = normalOut;

	// 2. CALCULATE THE TANGENT from texture space

	float s1 = v1.texCoord.x - v0.texCoord.x;
	float t1 = v1.texCoord.y - v0.texCoord.y;
	float s2 = v2.texCoord.x - v0.texCoord.x;
	float t2 = v2.texCoord.y - v0.texCoord.y;


	float tmp = 0.0f;
	if (fabsf(s1 * t2 - s2 * t1) <= 0.0001f)
	{
		tmp = 1.0f;
	}
	else
	{
		tmp = 1.0f / (s1 * t2 - s2 * t1);
	}

	XMFLOAT3 PF3, QF3;
	XMStoreFloat3(&PF3, P);
	XMStoreFloat3(&QF3, Q);

	tangent.x = (t2 * PF3.x - t1 * QF3.x);
	tangent.y = (t2 * PF3.y - t1 * QF3.y);
	tangent.z = (t2 * PF3.z - t1 * QF3.z);

	tangent.x = tangent.x * tmp;
	tangent.y = tangent.y * tmp;
	tangent.z = tangent.z * tmp;

	XMVECTOR vn = XMLoadFloat3(&normal);
	XMVECTOR vt = XMLoadFloat3(&tangent);

	// 3. CALCULATE THE BINORMAL
	// left hand system b = t cross n (rh would be b = n cross t)
	XMVECTOR vb = XMVector3Cross(vt, vn);

	vn = XMVector3Normalize(vn);
	vt = XMVector3Normalize(vt);
	vb = XMVector3Normalize(vb);

	XMStoreFloat3(&normal, vn);
	XMStoreFloat3(&tangent, vt);
	XMStoreFloat3(&binormal, vb);

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
		if (i == 8)
		{
			int x = 1;
		}
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