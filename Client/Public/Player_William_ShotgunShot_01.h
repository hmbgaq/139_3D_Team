#pragma once
#include "Player_State_Shotgun.h"


BEGIN(Client)

class CPlayer_William_ShotgunShot_01 : public CPlayer_State_Shotgun
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_William_ShotgunShot_01;

};

END