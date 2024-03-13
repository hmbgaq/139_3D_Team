#include "stdafx.h"
#include "Sniper_CoverLow_Side_L_Stop.h"
#include "Sniper_CoverLow_Side_L_AimPose.h"

void CSniper_CoverLow_Side_L_AimPose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Side_L_AimPose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	fTimeAcc += fTimeDelta;
	pActor->Look_At_Target();

	if (fTimeAcc >= 3.5f)
	{
		fTimeAcc = 0.f;
		return new CSniper_CoverLow_Side_L_Stop();
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverLow_Side_L_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
