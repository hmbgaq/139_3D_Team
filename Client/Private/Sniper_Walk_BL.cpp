#include "..\Public\Sniper_Walk_BL.h"

void CSniper_Walk_BL::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CBandit_Sniper>* CSniper_Walk_BL::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_Walk_BL::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
