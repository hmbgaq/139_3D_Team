#pragma once
#include "Player_State.h"

BEGIN(Client)

class CPlayer_State_Run : public CPlayer_State
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	virtual CState<CPlayer>* Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex) override;

private:
	_float m_fWalkDelay = { 0.2f };

};

END