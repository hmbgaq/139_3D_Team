#pragma once
#include "Infected_State_Dodge.h"


BEGIN(Client)

class CInfected_Dodge_F_01_TEMP : public CInfected_State_Dodge
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Dodge_F_01_TEMP;

};

END