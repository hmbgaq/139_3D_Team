#include "..\Public\BanditHeavy_KnockUp_Hit.h"

void CBanditHeavy_KnockUp_Hit::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_KnockUp_Hit::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_KnockUp_Hit::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
