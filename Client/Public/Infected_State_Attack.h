#pragma once
#include "Infected_State.h"

BEGIN(Client)

class CInfected_State_Attack : public CInfected_State
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

	void AttackState_Setting(CInfected* pActor);

protected:
	CWeapon* m_pWeapon = nullptr;
	CInfected::INFECTED_TYPE m_eType = CInfected::INFECTED_TYPE::INFECTED_END;

public:
	virtual CState<CInfected>* Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END