#include "Infected_Electrocute_Loop.h"
#include "Infected_Idle.h"

void CInfected_Electrocute_Loop::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
	pActor->Set_MonsterAttackState(true);
}

CState<CInfected>* CInfected_Electrocute_Loop::Update(CInfected* pActor, _float fTimeDelta)
{
	if (CInfected::INFECTED_TYPE::INFECTED_WASTER == pActor->Get_Info().Get_Type())
	{
		if (false == pActor->Is_ElectrocuteTime())
		{
			__super::Dead_State(pActor, fTimeDelta, g_iAnimIndex);
		}
	}
	else
	{
		if (false == pActor->Is_ElectrocuteTime())
		{
			return new CInfected_Idle();
		}
	}
	return nullptr;
}

void CInfected_Electrocute_Loop::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
