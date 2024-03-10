#include "..\Public\Infected_IdleAct_02.h"

void CInfected_IdleAct_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_IdleAct_02::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_IdleAct_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
