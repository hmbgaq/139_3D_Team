#pragma once
#include "Player_State_Finisher.h"


BEGIN(Client)

class CPlayer_Screamer_Finisher_ZapperPull_02 : public CPlayer_State_Finisher
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Screamer_Finisher_ZapperPull_02;

};

END