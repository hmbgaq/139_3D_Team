#include "..\Public\Infected_Walk_FL.h"

void CInfected_Walk_FL::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Walk_FL::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	
	return nullptr;
}

void CInfected_Walk_FL::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
