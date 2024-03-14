#include "stdafx.h"
#include "Sniper_CoverHigh_Side_L_AimPose.h"
#include "Sniper_CoverHigh_Side_L_Start.h"

void CSniper_CoverHigh_Side_L_Start::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Side_L_Start::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSniper_CoverHigh_Side_L_AimPose();
	}
	return nullptr;

}

void CSniper_CoverHigh_Side_L_Start::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
