#include "Infected_Scared_02.h"

void CInfected_Scared_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_Scared_02::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Scared_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
