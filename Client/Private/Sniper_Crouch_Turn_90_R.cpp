#include "Sniper_Crouch_Turn_90_R.h"

void CSniper_Crouch_Turn_90_R::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Crouch_Turn_90_R::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_Crouch_Turn_90_R::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
