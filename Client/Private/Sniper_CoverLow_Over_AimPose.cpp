#include "stdafx.h"
#include "Sniper_CoverLow_Over_Stop.h"
#include "Sniper_CoverLow_Over_AimPose.h"

void CSniper_CoverLow_Over_AimPose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Over_AimPose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	fTimeAcc += fTimeDelta;

	if (fTimeAcc >= 3.5f)
	{
		fTimeAcc = 0.f;
		return new CSniper_CoverLow_Over_Stop();
	}

	return nullptr;
}

void CSniper_CoverLow_Over_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
