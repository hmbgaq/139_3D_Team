#pragma once
#include "Infected_State_Attack.h"


BEGIN(Client)

class CInfected_Melee_RD_01 : public CInfected_State_Attack
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

private:
	CWeapon* m_pWeapon = nullptr;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Melee_RD_01;

};

END