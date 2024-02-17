#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEventActor abstract : public CGameObject
{
protected:
	CEventActor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEventActor(const CEventActor& rhs);
	virtual ~CEventActor() = default;

public:
	virtual void Activate() PURE;
	virtual _bool Is_Condition() PURE;


protected:
	virtual void Free() override;
};

END