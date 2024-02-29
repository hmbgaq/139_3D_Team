#include "..\Public\Infected_RelaxedIdleAct_03.h"

void CInfected_RelaxedIdleAct_03::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_RelaxedIdleAct_03::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_RelaxedIdleAct_03::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
