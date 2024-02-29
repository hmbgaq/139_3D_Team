#include "..\Public\Infected_HitNormal_FR_01_NEW.h"

void CInfected_HitNormal_FR_01_NEW::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_HitNormal_FR_01_NEW::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_HitNormal_FR_01_NEW::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
