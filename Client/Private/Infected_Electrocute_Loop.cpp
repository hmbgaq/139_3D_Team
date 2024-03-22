#include "..\Public\Infected_Electrocute_Loop.h"
#include "Infected_Idle.h"

void CInfected_Electrocute_Loop::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CInfected>* CInfected_Electrocute_Loop::Update(CInfected* pActor, _float fTimeDelta)
{
	if (false == pActor->Is_ElectrocuteTime()) 
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

void CInfected_Electrocute_Loop::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
