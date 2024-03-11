#pragma once
#include "Player_State_Attack.h"


BEGIN(Client)

class CPlayer_Uppercut_05v3_TEMP : public CPlayer_State_Attack
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Uppercut_05v3_TEMP;

};

END