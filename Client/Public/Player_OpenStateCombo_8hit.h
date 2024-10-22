#pragma once
#include "Player_State_Attack.h"


BEGIN(Client)

class CPlayer_OpenStateCombo_8hit : public CPlayer_State_Attack
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_OpenStateCombo_8hit;

public:
	CState<CPlayer>* Hit(CPlayer* pActor, _uint iHitCount, _uint iInputCount, _uint iAnimEnd);

};

END