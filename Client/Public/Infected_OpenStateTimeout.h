#pragma once
#include "Infected_State_Electrocute.h"


BEGIN(Client)

class CInfected_OpenStateTimeout : public CInfected_State_Electrocute
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_OpenStateTimeout;

};

END