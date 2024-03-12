#include "Sinper_Crouch_Start.h"

void CSinper_Crouch_Start::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSinper_Crouch_Start::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSinper_Crouch_Start::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
