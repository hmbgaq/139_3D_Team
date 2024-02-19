#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
END


BEGIN(Client)

class CPlayer;

class CTestEventWithPlayer : public IEvent
{
protected:
	CTestEventWithPlayer();
	virtual ~CTestEventWithPlayer() = default;

public:
	virtual void Activate() override;
	virtual _bool Activate_Condition() override;
	virtual _bool End_Condition() override;

public:
	void Set_Actor(CPlayer* pActor) {
		m_pActor = pActor;
	}

public:
	static CTestEventWithPlayer* Create(CPlayer* pActor);
	virtual void Free();

protected:
	CPlayer* m_pActor = { nullptr };

};

END