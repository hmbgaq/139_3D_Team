#pragma once
#include "Infected_State_Electrocute.h"


BEGIN(Client)

class CInfected_Electrocute_Loop : public CInfected_State_Electrocute
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	static const _uint g_iAnimIndex = CInfected::Infected_State::Infected_Electrocute_Loop;

private:
	class CEffect* m_pEffect = {nullptr};

};

END