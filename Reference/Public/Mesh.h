#pragma once

#include "VIBuffer.h"
#include "Model.h"
#include "MyAIMesh.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
protected:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	//! 모델 인스턴싱 전용
	_uint						Get_NumIndices() { return m_iNumIndices; } 
	_uint						Get_Stride() { return m_iStride; }
	D3D11_PRIMITIVE_TOPOLOGY	Get_Topology() const { return m_eTopology; }
	DXGI_FORMAT					Get_IndexFormat() const { return m_eIndexFormat; }

	ID3D11Buffer*				Get_VertexBuffer() { return m_pVB; }
	ID3D11Buffer*				Get_IndexBuffer() { return m_pIB; }
	
	//! 모델 인스턴싱 앤드

	//! 이펙트 메쉬 정점 위치 확인 테스트
	_uint		Get_NumVertices() { return m_iNumVertices; }
	_float3		Get_VerticePos(_int iIndex) { return m_pVertices[iIndex].vPosition; }
	_uint*		Get_Indices() { return m_pIndices; }
	//! 이펙트 메쉬 정점 위치 확인 테스트 끝
	
public:
    virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);
    

	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT			Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);
	
#ifdef _DEBUG
	_bool			Picking(RAY ray, _float3* out);
#endif // _DEBUG

	_char* Get_Name() {
		return m_szName;
	}

private:
	_char				m_szName[MAX_PATH];
	_uint				m_iMaterialIndex = { 0 } ;

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;

	VTXANIMMESH* m_pAnimVertices = { nullptr };
	VTXMESH*	 m_pVertices = { nullptr };
	_uint*		 m_pIndices = { nullptr };

private:
	HRESULT Ready_Vertices_NonAnim(CMyAIMesh pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertices_Anim(CMyAIMesh pAIMesh, const vector<class CBone*>& Bone);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END