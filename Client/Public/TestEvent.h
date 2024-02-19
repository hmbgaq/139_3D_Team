#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
END

BEGIN(Client)

class CTestEvent : public IEvent
{
protected:
	CTestEvent();
	virtual ~CTestEvent() = default;

public:
	virtual void Activate() override;
	virtual _bool Activate_Condition() override;
	virtual _bool End_Condition() override;

public:
	static CTestEvent* Create();
	virtual void Free();

};

END