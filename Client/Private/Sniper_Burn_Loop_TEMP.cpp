#include "stdafx.h"
#include "Sniper_Burn_Loop_TEMP.h"

void CSniper_Burn_Loop_TEMP::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Burn_Loop_TEMP::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_Burn_Loop_TEMP::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
