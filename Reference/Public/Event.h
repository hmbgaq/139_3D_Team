#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CEvent abstract : public CBase
{

public:
	CEvent(CGameObject* pEventActor, OnEvent OnEvnet, Condition Condition);
	virtual ~CEvent() = default;

public:
	_bool Activate();


private:
	OnEvent m_OnEvnet;
	Condition m_Condition;
	CGameObject* m_pEventActor = { nullptr };

public:
	virtual void Free() override;
};

END