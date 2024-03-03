#pragma once
#include "Bandit_Sniper_State_Walk.h"


BEGIN(Client)

class CSniper_Walk_F : public CBandit_Sniper_State_Walk
{
public:
	virtual void Initialize(CBandit_Sniper* pActor) override;
	virtual CState<CBandit_Sniper>* Update(CBandit_Sniper* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Sniper* pActor) override;

public:
	static const _uint g_iAnimIndex = CBandit_Sniper::Bandit_Sniper_State::Sniper_Walk_F;

};

END