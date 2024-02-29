#include "..\Public\Infected_SpawnFromCoffin0.h"

void CInfected_SpawnFromCoffin0::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnFromCoffin0::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnFromCoffin0::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
