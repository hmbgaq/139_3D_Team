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
	void Set_End() {
		m_bIsEnd = true;
	}

public:
	HRESULT Initialize();

public:
	virtual void Activate() PURE;
	virtual _bool Is_Condition() PURE;

protected:
	_bool m_bIsEnd = { false };

protected:
	virtual void Free() override;
};

END