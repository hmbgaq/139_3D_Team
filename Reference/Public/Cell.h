#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END};
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END};
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _float3*	Get_Point(POINT ePoint) const {
		return &m_vPoints[ePoint];
	}

	const _float3*	Get_Points() { return m_vPoints; }
	_int			Get_NeighborIndex(LINE eLine);

	_int			Get_CurrentIndex() {return m_iIndex; }
	void			Set_Point(POINT ePoint, _float3 vPoint);
	void			Set_Index(_uint iIndex) { m_iIndex = iIndex; }
	void			Set_PointY(POINT ePoint, _float fY);
	_float			Get_Height(const _float3& vPosition);

	const _float3*	Get_Normal(LINE eLine) const { return &m_vPoints[eLine]; }
	const _float3*	Get_Normals() { return m_vNormals; }

	_int			Get_Index() { return m_iIndex; }

	_bool			Is_Picking() { return m_bPicking; }
	void			Set_Picking(_bool bPicking) { m_bPicking = bPicking; }

	_float3			Get_Compare_Point(const _float3* pPoint);

	//!For. SlidingVector
	_bool			Is_Out(_fvector vWorldPos, _fvector vLook, _fmatrix WorldMatrix, _Inout_ _int* pNeighborIndex, _Inout_ _float4* pSliding);

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isIn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex);
	_bool isInRange(_fvector vPosition, _fmatrix WorldMatrix);
	void  Reset_Line();
	void  Update(_fmatrix WorldMatrix);

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

	_bool					m_bPicking = false;

#ifdef _DEBUG
private:	
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };


#endif

public:
	static CCell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END