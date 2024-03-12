	#pragma once
#include "Infected_State_Walk.h"


BEGIN(Client)

class CInfected_Walk_F : public CInfected_State_Walk
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Walk_F;

};

END