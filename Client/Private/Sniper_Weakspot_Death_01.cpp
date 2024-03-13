#include "Sniper_Weakspot_Death_01.h"

void CSniper_Weakspot_Death_01::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Weakspot_Death_01::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Death_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CSniper_Weakspot_Death_01::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
