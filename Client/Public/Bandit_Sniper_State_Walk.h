#pragma once
#include "Bandit_Sniper_State.h"

BEGIN(Client)

class CBandit_Sniper_State_Walk : public CBandit_Sniper_State
{
public:
	virtual void Initialize(CBandit_Sniper* pActor) override;
	virtual CState<CBandit_Sniper>* Update(CBandit_Sniper* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Sniper* pActor) override;

public:
	virtual CState<CBandit_Sniper>* Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END