#include "..\Public\Sniper_IdlePose.h"

void CSniper_IdlePose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CBandit_Sniper>* CSniper_IdlePose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_IdlePose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
