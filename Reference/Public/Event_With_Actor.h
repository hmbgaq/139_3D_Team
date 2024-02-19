#pragma once
#include "Engine_Defines.h"
#include "Event.h"

#define interface class

BEGIN(Engine)

interface ENGINE_DLL IEvent_With_Actor : public IEvent
{
//public:
//	IEvent_With_Actor();
//	virtual ~IEvent_With_Actor() = default;

public:
	//virtual void Activate() PURE;
	//virtual _bool Activate_Condition() PURE;
	//virtual _bool End_Condition() PURE;
	virtual _bool Is_End() override {
		if (nullptr == m_pActor)
			return true;

		m_bIsEnd = End_Condition();
		return m_bIsEnd;
	}

	void Set_Actor(IEvent* pActor) {
		m_pActor = pActor;
	}

protected:
	IEvent* m_pActor = { nullptr };

};

END