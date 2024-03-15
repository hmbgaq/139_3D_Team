#include "..\Public\Infected_HitNormalToStun_01_NEW.h"
#include "Infected_Stun_Loop_NEW.h"

void CInfected_HitNormalToStun_01_NEW::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_HitNormalToStun_01_NEW::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Stun_Loop_NEW();
	}

	return nullptr;
}

void CInfected_HitNormalToStun_01_NEW::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
