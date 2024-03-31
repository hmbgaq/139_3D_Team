#pragma once
#include "Bandit_Heavy_State_Normal.h"


BEGIN(Client)

class CBanditHeavy_Idle : public CBandit_Heavy_State_Normal
{
public:
	virtual void Initialize(CBandit_Heavy* pActor) override;
	virtual CState<CBandit_Heavy>* Update(CBandit_Heavy* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Heavy* pActor) override;

public:
	static const _uint g_iAnimIndex = CBandit_Heavy::Bandit_Heavy_State::BanditHeavy_Idle;

private:
	_float m_fDelay = { 0.f };

};

END