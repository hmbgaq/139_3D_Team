#pragma once
#include "Infected_State_Stun.h"


BEGIN(Client)

class CInfected_HitHeavyToStun_01_NEW : public CInfected_State_Stun
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_HitHeavyToStun_01_NEW;

};

END