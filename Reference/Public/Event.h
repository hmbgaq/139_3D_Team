#pragma once

#define Interface class

BEGIN(Engine) 

Interface ENGINE_DLL IEvent
{
//public:
//	IEvent();
//	virtual ~IEvent() = default;

public:
	virtual void Activate() PURE;
	virtual _bool Activate_Condition() PURE;
	virtual _bool End_Condition() PURE;
	virtual void Tick_Event(_float fTimeDelta) {};
	//virtual _bool Is_End() {
	//	m_bIsEnd = End_Condition();
	//	return m_bIsEnd;
	//}

//protected:
//	_bool m_bIsEnd = { false };
};

END