#pragma once
#include "Player_State_Slam.h"


BEGIN(Client)

class CPlayer_SlamDown_v3 : public CPlayer_State_Slam
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_SlamDown_v3;

private:
	_float m_fEffectTimeAcc = { 0.f };

};

END