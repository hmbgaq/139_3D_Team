#include "Sniper_Crouch_Trun_90_L.h"

void CSniper_Crouch_Trun_90_L::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Crouch_Trun_90_L::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_Crouch_Trun_90_L::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
