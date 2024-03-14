#include "stdafx.h"
#include "Sniper_CoverLow_Side_L_Stop.h"
#include "Sniper_CoverLow_Idle.h"

void CSniper_CoverLow_Side_L_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Side_L_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Sup_Normal_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CSniper_CoverLow_Side_L_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
