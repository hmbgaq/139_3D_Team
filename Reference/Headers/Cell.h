#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END};
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END};
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _float3* Get_Point(POINT ePoint) const {
		return &m_vPoints[ePoint];
	}	

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isIn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex);
	void Update(_fmatrix WorldMatrix);
	void SetUp_Neighbor(LINE eLine, CCell* pNeighborCell) {
		m_iNeighbors[eLine] = pNeighborCell->m_iIndex;		
	}

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_float3					m_vPoints[POINT_END] = {};
	_int					m_iNeighbors[LINE_END] = { -1, -1, -1 };
	_float3					m_vNormals[LINE_END] = {};
	_uint					m_iIndex = { 0 };

#ifdef _DEBUG
private:	
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };


#endif

public:
	static CCell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END