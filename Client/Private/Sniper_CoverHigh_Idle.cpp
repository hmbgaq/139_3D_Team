#include "..\Public\Sniper_CoverHigh_Idle.h"

void CSniper_CoverHigh_Idle::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Idle::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverHigh_Idle::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
