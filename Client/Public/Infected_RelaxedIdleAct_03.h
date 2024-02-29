#pragma once
#include "Infected_State_Idle.h"


BEGIN(Client)

class CInfected_RelaxedIdleAct_03 : public CInfected_State_Idle
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_RelaxedIdleAct_03;

};

END