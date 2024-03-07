#include "..\Public\Sniper_CoverLow_Over_Start.h"

void CSniper_CoverLow_Over_Start::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Over_Start::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_CoverLow_Over_Start::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
