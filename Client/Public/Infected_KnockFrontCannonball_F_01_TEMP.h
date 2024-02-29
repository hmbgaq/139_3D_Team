#pragma once
#include "Infected_State_Knock.h"


BEGIN(Client)

class CInfected_KnockFrontCannonball_F_01_TEMP : public CInfected_State_Knock
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_KnockFrontCannonball_F_01_TEMP;

};

END