#pragma once
#include "Player_State_Hit.h"


BEGIN(Client)

class CPlayer_HitPeriodic_03 : public CPlayer_State_Hit
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_HitPeriodic_03;

};

END