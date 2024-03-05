#pragma once
#include "Bandit_Sniper_State_Cover.h"


BEGIN(Client)

class CSniper_CoverHigh_Side_R_AimPose : public CBandit_Sniper_State_Cover
{
public:
	virtual void Initialize(CBandit_Sniper* pActor) override;
	virtual CState<CBandit_Sniper>* Update(CBandit_Sniper* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Sniper* pActor) override;

public:
	static const _uint g_iAnimIndex = CBandit_Sniper::Bandit_Sniper_State::Sniper_CoverHigh_Side_R_AimPose;

};

END