#pragma once
#include "Infected_State_Hit.h"

BEGIN(Client)

class CInfected_State_Electrocute : public CInfected_State_Hit
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	virtual CState<CInfected>* Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex) override;
	CState<CInfected>* Dead_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);

};

END