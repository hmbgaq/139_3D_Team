#pragma once
#include "Player_State_Walk.h"


BEGIN(Client)

class CPlayer_Sniper_WalkAim_FR : public CPlayer_State_Walk
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Sniper_WalkAim_FR;

};

END