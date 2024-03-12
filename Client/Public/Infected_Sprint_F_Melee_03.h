#pragma once
#include "Infected_State_Attack.h"

BEGIN(Client)

class CInfected_Sprint_F_Melee_03 : public CInfected_State_Attack
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Sprint_F_Melee_03;

};

END