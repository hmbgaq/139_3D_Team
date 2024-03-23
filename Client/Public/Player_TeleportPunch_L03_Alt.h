#pragma once
#include "Player_State_TeleportPunch.h"


BEGIN(Client)

class CPlayer_TeleportPunch_L03_Alt : public CPlayer_State_TeleportPunch
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_TeleportPunch_L03_Alt;

};

END