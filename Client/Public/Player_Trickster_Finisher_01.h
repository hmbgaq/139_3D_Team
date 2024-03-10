#pragma once
#include "Player_State_Finisher.h"


BEGIN(Client)

class CPlayer_Trickster_Finisher_01 : public CPlayer_State_Finisher
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Trickster_Finisher_01;

};

END