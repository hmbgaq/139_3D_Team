#pragma once
#include "Player_State_Gatling.h"


BEGIN(Client)

class CPlayer_Bandit_Gatling : public CPlayer_State_Gatling
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Bandit_Gatling;

};

END