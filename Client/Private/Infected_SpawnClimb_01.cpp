#include "..\Public\Infected_SpawnClimb_01.h"

void CInfected_SpawnClimb_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnClimb_01::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnClimb_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
