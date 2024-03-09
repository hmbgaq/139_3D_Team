#include "..\Public\Infected_Idle.h"

void CInfected_Idle::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Idle::Update(CInfected* pActor, _float fTimeDelta)
{
	

	if (pActor->Calc_Distance())
	{
		
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Idle::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
