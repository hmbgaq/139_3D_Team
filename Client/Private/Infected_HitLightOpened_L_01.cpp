#include "..\Public\Infected_HitLightOpened_L_01.h"

void CInfected_HitLightOpened_L_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_HitLightOpened_L_01::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_HitLightOpened_L_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
