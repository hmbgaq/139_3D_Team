#pragma once

#include "Base.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
public:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(const _float3* pVerticesPos, _Out_ _uint* pIndices, _Out_ _uint* pNumIndices);
	HRESULT Make_Neighbors();
private:
	_uint						m_iCorners[CORNER_END] = { 0 };
	_uint						m_iCenter = { 0 };
	shared_ptr<CQuadTree>		m_Children[CORNER_END] = { nullptr, nullptr, nullptr, nullptr };
	shared_ptr<CQuadTree>		m_Neighbors[NEIGHBOR_END] = { nullptr, nullptr, nullptr, nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

private:

	_bool isDraw(const _float3* pVerticesPos);

public:
	static shared_ptr<CQuadTree> Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END