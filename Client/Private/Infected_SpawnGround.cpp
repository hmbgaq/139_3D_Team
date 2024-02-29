#include "..\Public\Infected_SpawnGround.h"

void CInfected_SpawnGround::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnGround::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnGround::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
