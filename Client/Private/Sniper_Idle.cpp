#include "Sniper_Idle.h"

void CSniper_Idle::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false);
}

CState<CBandit_Sniper>* CSniper_Idle::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}
void CSniper_Idle::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
