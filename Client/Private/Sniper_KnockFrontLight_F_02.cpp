#include "..\Public\Sniper_KnockFrontLight_F_02.h"

void CSniper_KnockFrontLight_F_02::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_KnockFrontLight_F_02::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSniper_KnockFrontLight_F_02::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
