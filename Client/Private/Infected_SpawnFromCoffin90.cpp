#include "..\Public\Infected_SpawnFromCoffin90.h"

void CInfected_SpawnFromCoffin90::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnFromCoffin90::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnFromCoffin90::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
