#include "stdafx.h"
#include "Sniper_Idle.h"
#include "Sniper_CoverHigh_Side_R_Stop.h"

void CSniper_CoverHigh_Side_R_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Side_R_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSniper_Idle();
	}

	return nullptr;
}

void CSniper_CoverHigh_Side_R_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
