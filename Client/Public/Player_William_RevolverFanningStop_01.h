#pragma once
#include "Player_State_Revolver.h"


BEGIN(Client)

class CPlayer_William_RevolverFanningStop_01 : public CPlayer_State_Revolver
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_William_RevolverFanningStop_01;

};

END