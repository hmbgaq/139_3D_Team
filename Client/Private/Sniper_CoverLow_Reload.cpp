#include "stdafx.h"
#include "Sniper_CoverLow_Idle.h"
#include "Sniper_CoverLow_Reload.h"

void CSniper_CoverLow_Reload::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Reload::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSniper_CoverLow_Idle();
	}

	return nullptr;
}

void CSniper_CoverLow_Reload::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
