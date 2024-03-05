#include "..\Public\Sniper_CoverLow_Reload.h"

void CSniper_CoverLow_Reload::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Reload::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverLow_Reload::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
