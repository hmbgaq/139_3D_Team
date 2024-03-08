#include "..\Public\Sniper_CoverHigh_Side_L_Stop.h"

void CSniper_CoverHigh_Side_L_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Side_L_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverHigh_Side_L_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
