#include "..\Public\Infected_Hit_whip_end_down.h"

void CInfected_Hit_whip_end_down::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Hit_whip_end_down::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Hit_whip_end_down::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
