#pragma once
#include "Player_State_Crossbow.h"


BEGIN(Client)

class CPlayer_Crossbow_Ironsights_AimPose : public CPlayer_State_Crossbow
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Crossbow_Ironsights_AimPose;

};

END