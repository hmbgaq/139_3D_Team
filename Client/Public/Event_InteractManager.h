#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
END


BEGIN(Client)

class CPlayer;

class CEvent_InteractManager : public IEvent
{

public:
	enum INTERACT_TYPE { INTERACT_JUMP100, INTERACT_JUMP200, INTERACT_JUMP300, INTERACT_END };

public:
	typedef struct tagInteractDesc
	{
		INTERACT_TYPE eInteractType = INTERACT_END;
		_uint		  iCurrentLevelIndex = 0;
		_float3		  vColliderSize = {};
		_float3		  vColliderCenter = {};
		_float3		  vInteractPos = {};
	}INTERACT_DESC;



protected:
			 CEvent_InteractManager();
	virtual ~CEvent_InteractManager() = default;

public:
	virtual void Activate() override;
	virtual _bool Activate_Condition() override;
	virtual _bool End_Condition() override;

public:
	void Set_Actor(CPlayer* pActor) {
		m_pActor = pActor;
	}


public:
	static CEvent_InteractManager* Create(CPlayer* pActor);
	virtual void Free();

private:
	CPlayer* m_pActor = { nullptr };
	
private:
	_uint									m_iInteractSize = { 0 };
	
	
	vector<CCollider*>						m_vecInteractCollider;
	vector<INTERACT_DESC>					m_vecInteractDesc;

};

END