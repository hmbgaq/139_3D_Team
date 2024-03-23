#include "..\Public\Infected_OpenStatePull_F_01.h"

void CInfected_OpenStatePull_F_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false);
}

CState<CInfected>* CInfected_OpenStatePull_F_01::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_OpenStatePull_F_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
