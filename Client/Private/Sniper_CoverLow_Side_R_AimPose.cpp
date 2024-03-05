#include "..\Public\Sniper_CoverLow_Side_R_AimPose.h"

void CSniper_CoverLow_Side_R_AimPose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Side_R_AimPose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverLow_Side_R_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
