#include "..\Public\Infected_SpawnJumpOut_01.h"

void CInfected_SpawnJumpOut_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnJumpOut_01::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnJumpOut_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
