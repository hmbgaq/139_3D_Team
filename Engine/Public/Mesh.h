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


public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);

private:
	_char				m_szName[MAX_PATH];
	_uint				m_iMaterialIndex = { 0 } ;

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;

	VTXANIMMESH* m_pAnimVertices = { nullptr };
	VTXMESH* m_pVertices = { nullptr };
	_uint* m_pIndices = { nullptr };

private:
	HRESULT Ready_Vertices_NonAnim(CMyAIMesh pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertices_Anim(CMyAIMesh pAIMesh, const vector<class CBone*>& Bone);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END