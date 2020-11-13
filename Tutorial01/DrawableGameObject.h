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



using namespace DirectX;



class DrawableGameObject
{
public:

	struct CameraBuffer {
		XMFLOAT3 cameraPosition;
		float padding;
		XMFLOAT3 cameraDirection;
		float padding2;
	};

	struct ShaderData {
		ID3D11PixelShader* _pixelShader;
		ID3D11VertexShader* _vertexShader;
		ID3D11InputLayout* _inputLayout;
		CameraBuffer _cameraBuffer;
	};
	HRESULT								virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) = 0;
	void								virtual Update(float t) = 0;
	void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat);


	XMFLOAT4X4*							GetWorldMat() { return m_World; }
	ID3D11Buffer*						GetVertexBuffer() { return m_pVertexBuffer; }
	ID3D11Buffer*						GetIndexBuffer() { return m_pIndexBuffer; }
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

	void								virtual Release();
protected:
	
	void								SetWorldMatrix(XMFLOAT4X4* world);
	void								CalculateTangentBinormal2(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal);
	void								CalculateModelVectors(SimpleVertex* _vertices, int _vertexCount);
	XMFLOAT4X4*							m_World = nullptr;

	ID3D11Buffer*						m_pVertexBuffer = nullptr;
	ID3D11Buffer*						m_tangentBuffer = nullptr;
	ID3D11Buffer*						m_bitangentBuffer = nullptr;
	ID3D11Buffer*						m_pIndexBuffer = nullptr;
	ID3D11Buffer*						m_pConstantBuffer = nullptr;
	ID3D11Buffer*						m_pMaterialConstantBuffer = nullptr;
	ID3D11Buffer*						m_parallaxBuffer = nullptr;
	ID3D11Buffer*						m_pCameraBuffer = nullptr;

	ID3D11ShaderResourceView*			m_pTextureResourceView = nullptr;
	ID3D11SamplerState *				m_pSamplerLinear = nullptr;
	ID3D11SamplerState*					m_pSamplerNormal = nullptr;
	MaterialPropertiesConstantBuffer	m_material;
	XMFLOAT3							m_position = XMFLOAT3(0, 0, 0);
	ID3D11InputLayout*					m_inputLayout = nullptr;

	ID3D11ShaderResourceView*			m_albedoTexture =  nullptr;
	ID3D11ShaderResourceView*			m_normalTexture = nullptr;
	ID3D11ShaderResourceView*			m_parallaxTexture = nullptr;
	int									NUM_VERTICES = 0;
	float								m_parallaxBias;
	float								m_parallaxScale;
private:
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
};

