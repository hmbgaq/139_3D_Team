#pragma once
#include "Infected_State_Knock.h"


BEGIN(Client)

class CInfected_KnockUp_Low_Fixed : public CInfected_State_Knock
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_KnockUp_Low_Fixed;

};

END