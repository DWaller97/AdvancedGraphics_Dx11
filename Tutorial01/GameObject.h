#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>
#include "structures.h"
#include "Time.h"
#include "CameraManager.h"
#include "OBJLoader.h"
#include "ShaderManager.h"



using namespace DirectX;



class GameObject
{
public:

	GameObject();
	~GameObject();

	XMFLOAT3							m_position = XMFLOAT3(0, 0, 0);



	MeshData mesh;


	static void								CalculateTangentBinormal2(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal);
	static void								CalculateModelVectors(SimpleVertex* _vertices, int _vertexCount);

	HRESULT								virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void								virtual Update(float t);
	void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat);


	XMFLOAT4X4*							GetWorldMat() { return m_World; }
	ID3D11Buffer*						GetVertexBuffer() { return mesh.VertexBuffer; }
	ID3D11Buffer*						GetIndexBuffer() { return mesh.IndexBuffer; }
	ID3D11ShaderResourceView**			GetTextureResourceView() { return &m_pTextureResourceView; 	}
	XMFLOAT4X4*							GetTransform() { return m_World; }
	ID3D11SamplerState**				GetTextureSamplerState() { return &m_pSamplerLinear; }
	MaterialPropertiesConstantBuffer	GetMaterial() { return m_material;}
	void								SetPosition(XMFLOAT3 position);
	void								SetShaders(ShaderData shaderData);
	void								SetShader(ID3D11PixelShader* _pixelShader);
	void								SetShader(ID3D11VertexShader* _vertexShader);
	void								SetShaders(ID3D11VertexShader* _vertexShader, ID3D11PixelShader* _pixelShader);
	void								SetParallaxScale(float _scale);
	void								SetParallaxBias(float _bias);
	void								SetMesh(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords);
	void								SetAlbedoTexture(ID3D11ShaderResourceView* _resourceView);
	void								virtual Release();
protected:
	
	void								SetWorldMatrix(XMFLOAT4X4* world);

	XMFLOAT4X4*							m_World = nullptr;

	ID3D11Buffer*						m_pConstantBuffer = nullptr;
	ID3D11Buffer*						m_pMaterialConstantBuffer = nullptr;
	ID3D11Buffer*						m_parallaxBuffer = nullptr;

	ID3D11ShaderResourceView*			m_pTextureResourceView = nullptr;
	ID3D11SamplerState *				m_pSamplerLinear = nullptr;
	ID3D11SamplerState*					m_pSamplerNormal = nullptr;
	MaterialPropertiesConstantBuffer	m_material;
	ID3D11InputLayout*					m_inputLayout = nullptr;

	ID3D11ShaderResourceView*			m_albedoTexture =  nullptr;
	ID3D11ShaderResourceView*			m_normalTexture = nullptr;
	ID3D11ShaderResourceView*			m_parallaxTexture = nullptr;
	int									NUM_VERTICES = 0;
	float								m_parallaxBias;
	float								m_parallaxScale;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

};
