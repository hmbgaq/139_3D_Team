#include "..\Public\Infected_Stun_Loop_NEW.h"

void CInfected_Stun_Loop_NEW::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CInfected>* CInfected_Stun_Loop_NEW::Update(CInfected* pActor, _float fTimeDelta)
{

	return nullptr;
}

void CInfected_Stun_Loop_NEW::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
