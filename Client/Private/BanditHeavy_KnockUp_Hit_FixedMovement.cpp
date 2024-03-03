#include "..\Public\BanditHeavy_KnockUp_Hit_FixedMovement.h"

void CBanditHeavy_KnockUp_Hit_FixedMovement::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_KnockUp_Hit_FixedMovement::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_KnockUp_Hit_FixedMovement::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
