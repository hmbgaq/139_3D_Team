#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();
public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_WorldPlanes(_fvector vPoint, _float fRadius);
	_bool isIn_LocalPlanes(_fvector vPoint, _float fRadius);

	

private:
	class CGameInstance*		m_pGameInstance = { nullptr };

private:
	_float3			m_vOriginalPoints[8];
	_float4			m_WorldPlanes[6];
	_float4			m_LocalPlanes[6];

private:
	void Make_Planes(const _float3* pPoints, _Out_ _float4* pPlanes);

	


public:
	static CFrustum* Create();
	virtual void Free() override;
};

END