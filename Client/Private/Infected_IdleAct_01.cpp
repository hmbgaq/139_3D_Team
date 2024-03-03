#include "..\Public\Infected_IdleAct_01.h"

void CInfected_IdleAct_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_IdleAct_01::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_IdleAct_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
