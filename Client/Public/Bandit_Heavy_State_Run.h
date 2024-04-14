#pragma once
#include "Bandit_Heavy_State.h"

BEGIN(Client)

class CBandit_Heavy_State_Run : public CBandit_Heavy_State
{
public:
	virtual void Initialize(CBandit_Heavy* pActor) override;
	virtual CState<CBandit_Heavy>* Update(CBandit_Heavy* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Heavy* pActor) override;

public:
	virtual CState<CBandit_Heavy>* Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex) override;

private:
	_float m_fWalkDelay = { 0.2f };

};

END