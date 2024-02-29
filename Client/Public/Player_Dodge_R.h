#pragma once
#include "Player_State_Dodge.h"


BEGIN(Client)

class CPlayer_Dodge_R : public CPlayer_State_Dodge
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_Dodge_R;

};

END