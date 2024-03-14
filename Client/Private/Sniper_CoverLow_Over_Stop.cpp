#include "stdafx.h"
#include "Sniper_CoverLow_Over_Stop.h"
#include "Sniper_CoverLow_Idle.h"

void CSniper_CoverLow_Over_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Over_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSniper_CoverLow_Idle();
	}

	return nullptr;
}

void CSniper_CoverLow_Over_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
