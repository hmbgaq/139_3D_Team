#pragma once
#include "Infected_State_Run.h"


BEGIN(Client)

class CInfected_Run_FR45 : public CInfected_State_Run
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Run_FR45;

};

END