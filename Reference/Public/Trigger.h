#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CEventObject;

class ENGINE_DLL CTrigger abstract : public CGameObject
{
protected:
	CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	HRESULT Initialize(CEventObject* pActor);

public:
	virtual _bool Is_Condition() PURE;

protected:
	CEventObject* m_pActor = { nullptr };


protected:
	virtual void Free() override;
};

END