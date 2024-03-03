#pragma once
#include "Player_State_Rifle.h"


BEGIN(Client)

class CPlayer_Rifle_Ironsights_Fire : public CPlayer_State_Rifle
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Rifle_Ironsights_Fire;

	

};

END