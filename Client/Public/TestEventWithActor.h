#pragma once

#include "Client_Defines.h"
//#include "Event_With_Actor.h"
#include "Event.h"

BEGIN(Engine)
END

BEGIN(Client)

class CTestEventWithActor : public IEvent
{
protected:
	CTestEventWithActor();
	virtual ~CTestEventWithActor() = default;

public:
	virtual void Activate() override;
	virtual _bool Activate_Condition() override;
	virtual _bool End_Condition() override;

public:
	void Set_Actor(IEvent* pActor) {
		m_pActor = pActor;
	}

public:
	static CTestEventWithActor* Create(IEvent* pActor);
	virtual void Free();

protected:
	IEvent* m_pActor = { nullptr };

};

END