#include "stdafx.h"
#include "Sniper_CoverHigh_Side_R_AimPose.h"
#include "Sniper_CoverHigh_Side_R_Stop.h"

void CSniper_CoverHigh_Side_R_AimPose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Side_R_AimPose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	fTimeAcc += fTimeDelta;

	if (fTimeAcc >= 3.5f) /* 리스폰 하는 애니메이션이 끝난다면 */
	{
		cout << "AimPos End" << endl;
		fTimeAcc = 0.f;
		return new CSniper_CoverHigh_Side_R_Stop();
	}

	return nullptr;
}

void CSniper_CoverHigh_Side_R_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
