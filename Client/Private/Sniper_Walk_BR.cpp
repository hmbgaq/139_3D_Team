#include "..\Public\Sniper_Walk_BR.h"

void CSniper_Walk_BR::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CBandit_Sniper>* CSniper_Walk_BR::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_Walk_BR::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
