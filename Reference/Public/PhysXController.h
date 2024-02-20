#pragma once
#include "Component.h"

BEGIN(Engine)


class ENGINE_DLL CPhysXController :
	public CComponent, public PxControllerFilterCallback, public PxUserControllerHitReport, public PxQueryFilterCallback
{
private:
	CPhysXController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXController(const CPhysXController& rhs);
	virtual ~CPhysXController() = default;

public:
	_uint	Get_PControllerIndex() const { return m_iControllerIndex; }

private:
	_uint									m_iControllerIndex;

private:
	void Free();
};

END