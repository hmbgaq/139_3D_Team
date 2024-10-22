#pragma once
#include "Bandit_Sniper_State_Cover.h"


BEGIN(Client)

class CSniper_CoverLow_Over_AimPose : public CBandit_Sniper_State_Cover
{
public:
	virtual void Initialize(CBandit_Sniper* pActor) override;
	virtual CState<CBandit_Sniper>* Update(CBandit_Sniper* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Sniper* pActor) override;

private:
	_float fTimeAcc = 0.f;
	_float4 vTargetPos = {};
	_bool bReady = false;

public:
	static const _uint g_iAnimIndex = CBandit_Sniper::Bandit_Sniper_State::Sniper_CoverLow_Over_AimPose;

};

END